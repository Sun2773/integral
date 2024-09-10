#include "mainwindow.h"
#include "canbus.h"

#include "modbus.h"

#include "modbus_rtu_can_port.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
    this->resize(400, 480);

    pushButton = new QPushButton(this);

    pushButton->setText("这是一个按钮");
    pushButton->resize(200, 100);

    connect(pushButton, SIGNAL(clicked()), this, SLOT(pushButtonClicked()));
    beep = new Beep();

    // CanBus *bus = new CanBus(20000);

}

MainWindow::~MainWindow() {
}

void MainWindow::pushButtonClicked() {
    /* 设置主窗口的样式 2 */
    // this->setStyleSheet("QMainWindow { background-color: rgba(238, 122, 233, 100%); }");
    // if (!file.exists()) {
    //     return;
    // }
    // file.open(QIODevice::ReadWrite);
    // QString buf = file.read(1);
    qDebug() << "按钮点击了\n";
    // beep->sound(2);
    // this->hide();

    modbus_t* mb;
    int       ret;
    mb = modbus_new_rtu("can0", 100000, 'N', 8, 1);   // linux
    //    modbus_free(mb);
    ret = modbus_rtu_can_convert(mb);
    modbus_set_debug(mb,1);
    ret = modbus_connect(mb);
    // 设置从机地址，成功返回0, 否则返回-1
    ret = modbus_set_slave(mb, 4);
    uint16_t table[10];
    ret = modbus_read_registers(mb, 0, 10, table);

    qDebug() << ret;
    qDebug() << table;
}
