#include "slave.h"

Slave::Slave(void) {
    start();
}

/**
 * @brief  读取任务
 * @note
 * @retval None
 */
void Slave::run(void) {
    int ret;
    /* 创建一个Modbus-RTU通讯对象 */
    mb = modbus_new_rtu("can0", 100 * 1000, 'N', 8, 1);
    /* 将串口通讯转换为485通讯 */
    ret = modbus_rtu_can_convert(mb);
    /* 链接到端口 */
    ret = modbus_connect(mb);

    while (1) {
        // for (QList<SlaveDrive>::iterator item = DriveList.begin(); item != DriveList.end(); ++item) {
        //     item->Com_Start();
        //     ret = modbus_set_slave(mb, item->Slave);
        //     ret = modbus_read_registers(mb, item->RegStart, item->RegSize, (uint16_t*) item->Reg);
        //     if (ret >= 0) {
        //         item->Com_End();
        //     } else {
        //         item->Com_Err();
        //     }
        //     msleep(10);
        // }
        msleep(1000);
    }
}

/**
 * @brief  添加设备
 * @note
 * @param  drive: 设备对象
 * @retval None
 */
void Slave::drive_add(SlaveDrive* drive) {
    DriveList.append(*drive);
}

int Slave::drive_count(void) {
    return DriveList.count();
}

/**
 * @brief  从机通讯开始
 * @note
 * @retval None
 */
void SlaveDrive::Com_Start(void) {
    State = 2;
}

/**
 * @brief  从机通讯结束
 * @note
 * @retval None
 */
void SlaveDrive::Com_End(void) {
    State      = 1;
    Online     = 1;
    OfflineCnt = Outtime;
}

/**
 * @brief  从机通讯错误
 * @note
 * @retval None
 */
void SlaveDrive::Com_Err(void) {
    if (OfflineCnt > 0) {
        OfflineCnt--;
    } else if (Online != 0) {
        Online = 0;
    }
    if (Online != 0) {
        State = 3;
    } else {
        State = 4;
    }
    qDebug() << "通讯失败!!";
}
