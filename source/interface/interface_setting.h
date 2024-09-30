#pragma once

#include <QDebug>
#include <QFrame>
#include <QGridLayout>
#include <QGuiApplication>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QScreen>

#include "beep.h"
#include "interface_module.h"

class InterfaceSetting : public QFrame {
    Q_OBJECT

  public:
    InterfaceSetting(QWidget* parent = nullptr);
    ~InterfaceSetting();

    /* 模块管理界面 */
    InterfaceModule* interface_module;

  private:
    /* 标题 */
    QLabel* title;
    /* 网格布局 */
    QGridLayout* layout;
    /* 模块管理 */
    QPushButton* btn_module;
    /* 主机状态 */
    QPushButton* btn_machine;
    /* 版本管理 */
    QPushButton* btn_version;
    /* 信号 */
  signals:
    /* 槽 */
  public slots:
    /* 设置按钮点击事件 */
    void btn_module_clicked();
};
