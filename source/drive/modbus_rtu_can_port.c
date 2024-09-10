#include "modbus.h"

#include <linux/can.h>
#include <net/if.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

struct _modbus_backend_struct {
    unsigned int backend_type;
    unsigned int header_length;
    unsigned int checksum_length;
    unsigned int max_adu_length;
    void*        set_slave;
    void*        build_request_basis;
    void*        build_response_basis;
    void*        prepare_response_tid;
    void*        send_msg_pr;
    ssize_t (*send)(modbus_t* ctx, const uint8_t* req, int req_length);
    void* receive;
    ssize_t (*recv)(modbus_t* ctx, uint8_t* rsp, int rsp_length);
    void* check_integrity;
    void* pre_check_confirmation;
    int (*connect)(modbus_t* ctx);
    void* is_connected;
    void (*close)(modbus_t* ctx);
    int (*flush)(modbus_t* ctx);
    int (*select)(modbus_t* ctx, fd_set* rset, struct timeval* tv, int msg_length);
    void* free;
};

struct _modbus_struct {
    /* Slave address */
    int slave;
    /* Socket or file descriptor */
    int                            s;
    int                            debug;
    int                            error_recovery;
    int                            quirks;
    struct timeval                 response_timeout;
    struct timeval                 byte_timeout;
    struct timeval                 indication_timeout;
    struct _modbus_backend_struct* backend;
    void*                          backend_data;
};

struct _modbus_rtu_struct {
    /* 驱动名. */
    char* device;
    /* 波特率 */
    int baud;
    /* 数据位 */
    uint8_t data_bit;
    /* 停止位 */
    uint8_t stop_bit;
    /* 校验位 */
    char parity;
    /* Save old termios settings */
    struct termios old_tios;
    /* To handle many slaves on the same link */
    int confirmation_to_ignore;

    struct {
        uint8_t  buf[256];
        uint16_t r_p;
        uint16_t w_p;
    } FIFO;                        // 先入先出循环缓冲区
    pthread_t       recv_thread;   // 定义线程
    pthread_mutex_t mutex;         // 定义线程锁
    pthread_cond_t  cond;          // 定义条件变量
    int             recv_exit;     // 接收线程结束
};

static ssize_t _modbus_rtu_send(modbus_t* ctx, const uint8_t* req, int req_length) {
    struct _modbus_struct* _ctx  = (struct _modbus_struct*) ctx;
    struct can_frame       frame = {0};
    int                    ret;

    uint8_t* pack_max;   // 总包数
    uint8_t* pack_cnt;   // 当前包索引
    uint8_t* slave_id;   // 当前包索引
    uint8_t* send_cnt;   // 当前包索引

    /* 发送数据 */
    pack_cnt = (uint8_t*) &frame.can_id;    // 剩余包数
    pack_max = (uint8_t*) (pack_cnt + 1);   // 最大包数
    slave_id = (uint8_t*) (pack_max + 1);   // 首字ID
    send_cnt = (uint8_t*) (slave_id + 1);   // 发送次数
    /* 统计数据包数量 */
    *pack_max = req_length / 8;
    *pack_max += (req_length % 8 != 0);
    (*pack_max) -= 1;
    /* 复位当前包索引 */
    *pack_cnt = 0;
    *slave_id = *req;
    /* 根据剩余包数发送报文 */
    while ((*pack_cnt) <= (*pack_max)) {
        /* 写入8字节报文 */
        for (frame.can_dlc = 0; frame.can_dlc < 8 && req_length > 0; frame.can_dlc++, req_length--) {
            frame.data[frame.can_dlc] = *req++;
        }
        for (*send_cnt = 0; (*send_cnt) < 3; (*send_cnt)++) {
            /* 对帧ID进行掩码处理 */
            frame.can_id &= 0x1FFFFFFF;
            frame.can_id |= CAN_EFF_FLAG;
            ret = write(_ctx->s, &frame, sizeof(frame));   // 发送数据
            if (sizeof(frame) == ret) {                    // 如果ret不等于帧长度，就说明发送失败
                break;
            }
        }
        (*pack_cnt)++;
    }

    return (((*pack_cnt) - 1) * 8 + frame.can_dlc);
}

/**
 * @brief  CAN通讯接收任务
 * @note
 * @retval None
 */
static void* _modbus_rtu_can_recv_task(void* p) {
    struct can_frame           frame   = {0};
    struct _modbus_struct*     _ctx    = (struct _modbus_struct*) p;
    struct _modbus_rtu_struct* ctx_rtu = (struct _modbus_rtu_struct*) _ctx->backend_data;
    int                        res     = 0;

    while (ctx_rtu->recv_exit >= 0) {
        /* 读取一帧数据 */
        if ((res = read(_ctx->s, &frame, sizeof(struct can_frame))) <= 0) {
            continue;
        }
        /* 结束线程 */
        if (ctx_rtu->recv_exit < 0) {
            break;
        }
        /* 锁定资源 */
        pthread_mutex_lock(&ctx_rtu->mutex);
        /* 复制数据 */
        for (uint8_t i = 0; (i < frame.can_dlc); i++) {
            ctx_rtu->FIFO.buf[ctx_rtu->FIFO.w_p++] = frame.data[i];
            if (ctx_rtu->FIFO.w_p >= sizeof(ctx_rtu->FIFO.buf)) {
                ctx_rtu->FIFO.w_p = 0;
            }
            if (ctx_rtu->FIFO.w_p == ctx_rtu->FIFO.r_p) {
                ctx_rtu->FIFO.r_p += 1;
            }
            if (ctx_rtu->FIFO.r_p >= sizeof(ctx_rtu->FIFO.buf)) {
                ctx_rtu->FIFO.r_p = 0;
            }
        }
        /* 解锁资源 */
        pthread_mutex_unlock(&ctx_rtu->mutex);
        /* 向条件变量发送信号 */
        pthread_cond_signal(&ctx_rtu->cond);
    }
    /* 线程异常终止 */
    return NULL;
}

static ssize_t _modbus_rtu_recv_FIFO_select(modbus_t* ctx) {
    struct _modbus_struct*     _ctx    = (struct _modbus_struct*) ctx;
    struct _modbus_rtu_struct* ctx_rtu = (struct _modbus_rtu_struct*) _ctx->backend_data;
    ssize_t                    length  = 0;

    if (ctx_rtu->FIFO.w_p > ctx_rtu->FIFO.r_p) {
        length = ctx_rtu->FIFO.w_p - ctx_rtu->FIFO.r_p;
    } else if (ctx_rtu->FIFO.w_p < ctx_rtu->FIFO.r_p) {
        length = sizeof(ctx_rtu->FIFO.buf) - ctx_rtu->FIFO.r_p + ctx_rtu->FIFO.w_p;
    }
    return length;
}

static ssize_t _modbus_rtu_recv(modbus_t* ctx, uint8_t* rsp, int rsp_length) {
    struct _modbus_struct*     _ctx    = (struct _modbus_struct*) ctx;
    struct _modbus_rtu_struct* ctx_rtu = (struct _modbus_rtu_struct*) _ctx->backend_data;
    ssize_t                    res     = 0;
    struct timespec            start_time, end_time;
    /* 锁定资源 */
    pthread_mutex_lock(&ctx_rtu->mutex);
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    /* 首次等待500ms */
    end_time.tv_sec  = 0;
    end_time.tv_nsec = 500 * 1e6;
    /* 检查缓冲区内是否有足够的数据 */
    while (_modbus_rtu_recv_FIFO_select(ctx) < rsp_length) {
        /* 等待通知 */
        pthread_cond_timedwait(&ctx_rtu->cond, &ctx_rtu->mutex, &end_time);
        /* 计算已经等待的时间 */
        clock_gettime(CLOCK_MONOTONIC, &end_time);
        double elapsed = ((end_time.tv_sec - start_time.tv_sec) * 1000.0 +
                          (end_time.tv_nsec - start_time.tv_nsec) / 1e6);
        /* 超时直接结束 */
        if (elapsed > 500) {
            break;
        }
        /* 重新计算下次等待时间 */
        end_time.tv_sec  = 0;
        end_time.tv_nsec = (500 - elapsed) * 1e6;
    }
    /* 缓冲区有足够的字节数 */
    if (_modbus_rtu_recv_FIFO_select(ctx) >= rsp_length) {
        /* 将数据读取出来 */
        for (res = 0; (res < rsp_length) && (ctx_rtu->FIFO.r_p != ctx_rtu->FIFO.w_p); res++) {
            rsp[res] = ctx_rtu->FIFO.buf[ctx_rtu->FIFO.r_p++];
            if (ctx_rtu->FIFO.r_p >= sizeof(ctx_rtu->FIFO.buf)) {
                ctx_rtu->FIFO.r_p = 0;
            }
        }
    }
    /* 解锁资源 */
    pthread_mutex_unlock(&ctx_rtu->mutex);
    return res;
}

/* POSIX */
static int _modbus_rtu_connect(modbus_t* ctx) {
    if (ctx == NULL) {
        return -1;
    }
    struct _modbus_struct*     _ctx     = (struct _modbus_struct*) ctx;
    struct _modbus_rtu_struct* ctx_rtu  = (struct _modbus_rtu_struct*) _ctx->backend_data;
    struct ifreq               ifr      = {0};
    struct sockaddr_can        can_addr = {0};
    int                        ret;
    char                       cmd[100];
    /* 初始化惨接口 */
    sprintf(cmd, "ifconfig %s down", ctx_rtu->device);
    system(cmd);
    sprintf(cmd, "ip link set %s up type can bitrate %d restart-ms 100", ctx_rtu->device, ctx_rtu->baud);
    system(cmd);
    /* 打开套接字 */
    _ctx->s = socket(PF_CAN, SOCK_RAW, CAN_RAW);

    if (_ctx->s < 0) {
        return -1;
    }

    /* 指定can0设备 */
    strcpy(ifr.ifr_name, ctx_rtu->device);
    ret = ioctl(_ctx->s, SIOCGIFINDEX, &ifr);

    can_addr.can_family  = AF_CAN;
    can_addr.can_ifindex = ifr.ifr_ifindex;

    /* 将can0与套接字进行绑定 */
    ret = bind(_ctx->s, (struct sockaddr*) &can_addr, sizeof(can_addr));
    if (0 > ret) {
        close(_ctx->s);
        _ctx->s = -1;
    }

    return 0;
}

static void _modbus_rtu_close(modbus_t* ctx) {
    struct _modbus_struct*     _ctx    = (struct _modbus_struct*) ctx;
    struct _modbus_rtu_struct* ctx_rtu = (struct _modbus_rtu_struct*) _ctx->backend_data;
    if (_ctx->s >= 0) {
        /* 关闭套接字 */
        close(_ctx->s);
        /* 锁定资源 */
        pthread_mutex_lock(&ctx_rtu->mutex);
        ctx_rtu->recv_exit = -1;
        _ctx->s            = -1;
        /* 解锁资源 */
        pthread_mutex_unlock(&ctx_rtu->mutex);
    }
}

static int _modbus_rtu_flush(modbus_t* ctx) {
    struct _modbus_struct*     _ctx    = (struct _modbus_struct*) ctx;
    struct _modbus_rtu_struct* ctx_rtu = (struct _modbus_rtu_struct*) _ctx->backend_data;
    /* 锁定资源 */
    pthread_mutex_lock(&ctx_rtu->mutex);
    /* 清空先入先出缓冲区 */
    memset(&ctx_rtu->FIFO, 0, sizeof(ctx_rtu->FIFO));
    /* 解锁资源 */
    pthread_mutex_unlock(&ctx_rtu->mutex);
    return 0;
}

static int _modbus_rtu_select(modbus_t* ctx, fd_set* rset, struct timeval* tv, int length_to_read) {
    int s_rc;
    (void) rset;
    (void) tv;
    (void) length_to_read;
    /* 检查剩余可用字节数 */
    s_rc = _modbus_rtu_recv_FIFO_select(ctx);
    return s_rc;
}

int modbus_rtu_can_convert(modbus_t* ctx) {
    if (ctx == NULL) {
        return -1;
    }
    struct _modbus_struct*     _ctx    = (struct _modbus_struct*) ctx;
    struct _modbus_rtu_struct* ctx_rtu = malloc(sizeof(struct _modbus_rtu_struct));
    memset(ctx_rtu, 0, sizeof(struct _modbus_rtu_struct));
    /* 拷贝旧参数 */
    ctx_rtu->device                 = ((struct _modbus_rtu_struct*) _ctx->backend_data)->device;
    ctx_rtu->baud                   = ((struct _modbus_rtu_struct*) _ctx->backend_data)->baud;
    ctx_rtu->data_bit               = ((struct _modbus_rtu_struct*) _ctx->backend_data)->data_bit;
    ctx_rtu->stop_bit               = ((struct _modbus_rtu_struct*) _ctx->backend_data)->stop_bit;
    ctx_rtu->parity                 = ((struct _modbus_rtu_struct*) _ctx->backend_data)->parity;
    ctx_rtu->old_tios               = ((struct _modbus_rtu_struct*) _ctx->backend_data)->old_tios;
    ctx_rtu->confirmation_to_ignore = ((struct _modbus_rtu_struct*) _ctx->backend_data)->confirmation_to_ignore;
    /* 释放旧内存 */
    free((void*) _ctx->backend_data);
    /* 保存新参数 */
    _ctx->backend_data = ctx_rtu;
    /* 重新定义接口函数 */
    _ctx->backend->connect = _modbus_rtu_connect;
    _ctx->backend->close   = _modbus_rtu_close;
    _ctx->backend->send    = _modbus_rtu_send;
    _ctx->backend->recv    = _modbus_rtu_recv;
    _ctx->backend->flush   = _modbus_rtu_flush;
    _ctx->backend->select  = _modbus_rtu_select;
    /* 保持接收线程 */
    ctx_rtu->recv_exit = 0;
    /* 创建一个接收线程 */
    pthread_create(&ctx_rtu->recv_thread, NULL, _modbus_rtu_can_recv_task, (void*) _ctx);

    return 0;
}
