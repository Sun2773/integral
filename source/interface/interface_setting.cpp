#include "interface_setting.h"

#include "mainwindow.h"

InterfaceSetting::InterfaceSetting(QWidget* parent)
    : QFrame(parent) {
    _MainWindow->addInterface(this);
    { /* 指定QSS样式 */
        QFile file(":/style/interface_setting.qss");
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
        this->setObjectName("interface_setting");
        this->setGeometry(70, 50, parent->size().width() - 70 - 10, parent->size().height() - 50 - 10);
    }
    {
        layout = new QGridLayout(this);
        layout->setGeometry(this->rect());
        layout->setRowMinimumHeight(0, 40);
    }
    {
        /* 标题 */
        title = new QLabel("系统", this);
        title->resize(100, 30);
        title->move((this->size().width() - title->size().width()) / 2, 5);
        QFont font("Arial", 20);                // 设置字体为 Arial，大小为 20
        title->setFont(font);                   //
        title->setAlignment(Qt::AlignCenter);   // 设置垂直居中对齐
        layout->addWidget(title, 0, 0, 1, 3);
    }
    {
        btn_module = new QPushButton("模块管理", this);
        layout->addWidget(btn_module, 1, 0);
        connect(btn_module, &QPushButton::clicked, this, &InterfaceSetting::btn_module_clicked);
    }
    {
        btn_machine = new QPushButton("主机状态", this);
        layout->addWidget(btn_machine, 1, 1);
    }
    {
        btn_version = new QPushButton("版本", this);
        layout->addWidget(btn_version, 1, 2);
    }
    {
        interface_module = new InterfaceModule(_MainWindow);
        interface_module->hide();

    }
}

InterfaceSetting::~InterfaceSetting() {
}

void InterfaceSetting::btn_module_clicked() {
    _MainWindow->changeInterface(interface_module);
}
