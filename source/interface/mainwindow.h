#pragma once

#include <QDateTime>
#include <QDebug>
#include <QFrame>
#include <QGuiApplication>
#include <QLabel>
#include <QList>
#include <QMainWindow>
#include <QPushButton>
#include <QScreen>
#include <QTextBlock>
#include <QTimer>
#include <QVBoxLayout>

#include "beep.h"
#include "interface_main.h"
#include "interface_setting.h"


/* 标题栏对象定义 */
class InterfaceTitle : public QFrame {
    Q_OBJECT

  public:
    /* 构造函数 */
    InterfaceTitle(QWidget* parent = nullptr);
    /* 析构函数 */
    ~InterfaceTitle();

  private:
    /* 日期时间 */
    QLabel* datetime;
    /* 定时器 */
    QTimer* timer;
    /* CPU温度*/
    QLabel* cpu_temp;

    /* 槽 */
  public slots:
    /* 定时任务 */
    void cron_job();
};

/* 侧边栏对象定义 */
class InterfaceSidebar : public QFrame {
    Q_OBJECT

  public:
    /* 构造函数 */
    InterfaceSidebar(QWidget* parent = nullptr);
    /* 析构函数 */
    ~InterfaceSidebar();

  private:
    /* 设置按钮 */
    QPushButton* setting_button;
    /* 主页按钮 */
    QPushButton* home_button;
    /* 槽 */
  public slots:
    /* 设置按钮点击事件 */
    void setting_button_Clicked();
    /* 主页按钮点击事件 */
    void home_button_Clicked();
};

/* 主窗口对象定义 */
class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    /* 构造函数 */
    MainWindow(QWidget* parent = nullptr);
    /* 析构函数 */
    ~MainWindow();
    /* 主界面 */
    InterfaceMain* interface_main;
    /* 设置界面 */
    InterfaceSetting* interface_setting;
    /* 界面标题栏 */
    InterfaceTitle* title;
    
    void addInterface(QFrame* interface);
    void removeInterface(QFrame* interface);
    void changeInterface(QFrame* interface);

  private:
    /* 界面侧边栏 */
    InterfaceSidebar* sidebar;
    Beep*             beep;

    /* 从机列表 */
    QList<QFrame*> InterfaceList;
    
    /* 信号 */
  signals:
    void pushButtonSignal();
    /* 槽 */
  public slots:
};

extern MainWindow* _MainWindow;

