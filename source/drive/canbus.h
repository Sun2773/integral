#pragma once

#include <QCanBus>
#include <QCanBusDevice>
#include <QDebug>
#include <QMainWindow>
class CanBus : public QObject {
    Q_OBJECT

  public:
    CanBus(int baud);

  private:
    QCanBusDevice* device;   // CAN设备驱动

  private slots:
    /* 接收消息 */
    void receivedFrames();
    /* 发送消息 */
    void sendFrame();
};
