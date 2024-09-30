#include "mainwindow.h"
#include "canbus.h"

#include <QFile>
#include "slave.h"

/******************** 48路继电器驱动定义 ********************/

/* 48路继电器驱动板寄存器结构体定义 */
typedef struct {
    uint16_t Type;          // 板类型
    uint16_t Version;       // 程序版本号
    uint16_t RelayOut[3];   // 继电器输出
} Relay48BoardRegister_T;

/* 48路继电器驱动板寄存器 */
Relay48BoardRegister_T Relay48BoardRegister[1];

MainWindow* _MainWindow;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
    QList<QScreen*> list_screen = QGuiApplication::screens();
#if __arm__
    /* 重设大小 */
    this->resize(list_screen.at(0)->geometry().width(),
                 list_screen.at(0)->geometry().height());
#else
    /* 否则则设置主窗体大小为 800x480 */
    this->resize(800, 480);
#endif
    _MainWindow = this;
    { /* 指定QSS样式 */
        QFile file(":/style/framework.qss");
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
    { /* 创建标题栏 */
        title = new InterfaceTitle(this);
    }
    { /* 创建侧边栏 */
        sidebar = new InterfaceSidebar(this);
    }
    {
        /* 创建主界面并显示 */
        interface_main = new InterfaceMain(this);
        interface_main->show();
    }
    {
        /* 创建设置界面并隐藏 */
        interface_setting = new InterfaceSetting(this);
        interface_setting->hide();
    }
}

MainWindow::~MainWindow() {
}

InterfaceTitle ::InterfaceTitle(QWidget* parent)
    : QFrame(parent) {
    { /* 标题栏属性 */
        this->setObjectName("title");
        this->setGeometry(0, 0, parent->size().width(), 40);
        this->setFrameShape(QFrame::StyledPanel);
        this->setFrameShadow(QFrame::Raised);
    }
    { /* 标题时间 */
        datetime = new QLabel(this);
        datetime->setGeometry(5,
                              (this->size().height() - 30) / 2,
                              260,
                              30);
        QFont font("Arial", 20);                    // 设置字体为 Arial，大小为 20
        datetime->setFont(font);                    //
        datetime->setAlignment(Qt::AlignVCenter);   // 设置垂直居中对齐
    }
    { /* 标题时间 */
        cpu_temp = new QLabel(this);
        cpu_temp->setGeometry(500,
                              (this->size().height() - 30) / 2,
                              100,
                              30);
        QFont font("Arial", 20);                    // 设置字体为 Arial，大小为 20
        cpu_temp->setFont(font);                    //
        // cpu_temp->setAlignment(Qt::AlignCenter);   // 设置垂直居中对齐
        cpu_temp->setStyleSheet(
            "background-color: rgba(0, 240, 240, 100%);"
        );
    }
    { /* 定时任务 */
        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &InterfaceTitle::cron_job);
        timer->start(200);   // 启动定时器，每秒触发一次
    }
}

InterfaceTitle::~InterfaceTitle() {
}

void InterfaceTitle::cron_job() {
    /* 获取当前系统时间 */
    QDateTime currentDateTime = QDateTime::currentDateTime();
    /* 格式化时间字符串 */
    QString currentTimeString = currentDateTime.toString("yyyy-MM-dd hh:mm:ss");
    /* 更新时间到屏幕 */
    datetime->setText(currentTimeString);

    { /* 获取温度 */
        QFile file("/sys/class/thermal/thermal_zone0/temp");
        /* 判断文件是否存在 */
        if (file.exists()) {
            /* 以只读的方式打开 */
            file.open(QFile::ReadOnly);
            /* 以字符串的方式保存读出的结果 */
            QString str = QLatin1String(file.readAll());
            int temp = str.toInt();
            str = QString("%1℃").arg(temp/1000.0f,0,'f',1);
            cpu_temp->setText(str);
            /* 关闭文件 */
            file.close();
        }
    }
}

InterfaceSidebar ::InterfaceSidebar(QWidget* parent)
    : QFrame(parent) {
    { /* 侧边栏属性 */
        this->setObjectName("sidebar");
        this->setGeometry(0, 40, 60, parent->size().height() - 40);
        this->setFrameShape(QFrame::StyledPanel);
        this->setFrameShadow(QFrame::Raised);
    }
    { /* 设置按钮 */
        setting_button = new QPushButton(this);
        setting_button->setObjectName("setting_button");
        setting_button->resize(48, 48);
        setting_button->move((this->size().width() - setting_button->size().width()) / 2,
                             (this->size().height() - setting_button->size().height()) / 2 + 50);
        connect(setting_button, SIGNAL(clicked()), this, SLOT(setting_button_Clicked()));
    }
    { /* 主页按钮 */
        home_button = new QPushButton(this);
        home_button->setObjectName("home_button");
        home_button->resize(48, 48);
        home_button->move((this->size().width() - home_button->size().width()) / 2,
                          (this->size().height() - home_button->size().height()) / 2 - 50);
        connect(home_button, SIGNAL(clicked()), this, SLOT(home_button_Clicked()));
    }
}

InterfaceSidebar::~InterfaceSidebar() {
}

void InterfaceSidebar::setting_button_Clicked() {
    _MainWindow->changeInterface(_MainWindow->interface_setting);
}

void InterfaceSidebar::home_button_Clicked() {
    _MainWindow->changeInterface(_MainWindow->interface_main);
}

void MainWindow::addInterface(QFrame* interface) {
    InterfaceList.append(interface);
}

void MainWindow::removeInterface(QFrame* interface) {
    InterfaceList.removeOne(interface);
}
void MainWindow::changeInterface(QFrame* interface) {
    for (int i = 0; i < InterfaceList.size(); i++) {
        if (InterfaceList[i] == interface) {
            InterfaceList[i]->show();
        } else {
            InterfaceList[i]->hide();
        }
    }
}