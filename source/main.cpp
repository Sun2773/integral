#include "mainwindow.h"

#include <QApplication>
#include <QVirtualKeyboardViewManager>
#include "module.h"

#include "slave.h"

Slave* slaveDrive;

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);

    // 启用虚拟键盘
    QVirtualKeyboardViewManager::instance()->setEnabled(true);

    slaveDrive = new Slave();

    Module module;
    module.count();

    MainWindow w;
    w.show();

    return a.exec();
}
