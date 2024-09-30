#include "interface_main.h"

#include "mainwindow.h"
InterfaceMain::InterfaceMain(QWidget* parent)
    : QFrame(parent) {
    _MainWindow->addInterface(this);
    { /* 指定QSS样式 */
        QFile file(":/style/interface_main.qss");
        /* 判断文件是否存在 */
        if (file.exists()) {
            /* 以只读的方式打开 */
            file.open(QFile::ReadOnly);
            /* 以字符串的方式保存读出的结果 */
            QString styleSheet = QLatin1String(file.readAll());
            /* 设置全局样式 */
            this->setStyleSheet(styleSheet);
            /* 关闭文件 */
            file.close();
        }
    }
    { /* 窗口属性栏属性 */
        this->setObjectName("interface_main");
        this->setGeometry(70, 50, parent->size().width() - 70 - 10, parent->size().height() - 50 - 10);
    }
}

InterfaceMain::~InterfaceMain() {
}
