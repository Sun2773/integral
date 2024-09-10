#include "canbus.h"

CanBus::CanBus(int baud) {
    /* 使用系统命令关闭CAN接口 */
    system("ifconfig can0 down");
    /* 使用系统命令重新打开CAN接口并初始化波特率 */
    system("ip link set up can0 type can bitrate 20000 restart-ms 100");

    QString errorString;
    /* 实例化驱动 */
    device = QCanBus::instance()->createDevice("socketcan", "can0", &errorString);
    /* 实例化失败 */
    if (device == NULL) {
        qDebug() << "can0实例化失败";
        return;
    }
    /* 连接CAN */
    if (device->connectDevice() == false) {
        qDebug() << "can0链接失败";
        delete device;
        device = nullptr;
        return;
    }
    /* 链接接收信号到接收处理函数 */
    connect(device, SIGNAL(framesReceived()), this, SLOT(receivedFrames()));
}

/* 接收消息 */
void CanBus::receivedFrames() {
    if (device == nullptr)
        return;

    /* 读取帧 */
    while (device->framesAvailable()) {
        const QCanBusFrame frame = device->readFrame();
        QString            view;
        if (frame.frameType() == QCanBusFrame::ErrorFrame)
            view = device->interpretErrorFrame(frame);
        else
            view = frame.toString();
        qDebug() << view;
        // const QString time = QString::fromLatin1("%1.%2  ")
        //         .arg(frame.timeStamp()
        //              .seconds(), 10, 10, QLatin1Char(' '))
        //         .arg(frame.timeStamp()
        //              .microSeconds() / 100, 4, 10, QLatin1Char('0'));

        // const QString flags = frameFlags(frame);
        // /* 接收消息框追加接收到的消息 */
        // textBrowser->insertPlainText(time + flags + view + "\n");
    }
}

/* 发送消息 */
void CanBus::sendFrame() {
    // if (!device)
    //     return;
    // /* 读取QLineEdit的文件 */
    // QString str = lineEdit->text();
    // QByteArray data = 0;
    // QString strTemp = nullptr;
    // /* 以空格分隔lineEdit的内容，并存储到字符串链表中 */
    // QStringList strlist = str.split(' ');
    // for (int i = 1; i < strlist.count(); i++) {
    //     strTemp = strTemp + strlist[i];
    // }
    // /* 将字符串的内容转为QByteArray类型 */
    // data = QByteArray::fromHex(strTemp.toLatin1());

    // bool ok;
    // /* 以16进制读取要发送的帧内容里第一个数据，并作为帧ID */
    // int framId = strlist[0].toInt(&ok, 16);
    // QCanBusFrame frame = QCanBusFrame(framId, data);
    // /* 写入帧 */
    // device->writeFrame(frame);
}
