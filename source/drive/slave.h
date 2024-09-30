#pragma once

#include <QCanBus>
#include <QCanBusDevice>
#include <QDebug>
#include <QList>
#include <QMainWindow>
#include <QMutex>
#include <QThread>

#include "modbus.h"
#include "modbus_rtu_can_port.h"

class SlaveDrive {
  public:
    uint8_t  Slave;       // 从机地址
    QString  Name;        // 设备名称
    uint16_t RegSize;     // 寄存器大小
    uint16_t RegStart;    // 寄存器起始地址
    void*    Reg;         // 寄存器指针
    uint16_t SlaveType;   // 设备类型
    uint16_t Outtime;     // 超时计数

    /* 从机通讯开始 */
    void Com_Start(void);
    /* 从机通讯结束 */
    void Com_End(void);
    /* 从机通讯错误 */
    void Com_Err(void);

  private:
    uint8_t  Online;       // 在线状态 0 未在线 1 在线
    uint16_t OfflineCnt;   // 离线计数
    uint8_t  State;        // 通信状态 0 未启用 1 通信正常 2 正在通信 3 通信异常 4 通讯离线

    uint16_t DriveType;      // 设备类型
    uint16_t DriveVersion;   // 设备版本号
};

class Slave : public QThread {
    Q_OBJECT

  public:
    Slave(void);
    /* 添加一个设备 */
    void drive_add(SlaveDrive* drive);

    int drive_count();

  private:
    /* Modbus对象 */
    modbus_t* mb;
    /* 运行任务重载 */
    void run(void) override;
    /* 从机列表 */
    QList<SlaveDrive> DriveList;

  private slots:
};
