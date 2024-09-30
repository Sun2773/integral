#pragma once

#include <QDebug>
#include <QDialog>
#include <QFrame>
#include <QGridLayout>
#include <QGuiApplication>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>
#include <QScreen>
#include <QSignalMapper>
#include <QStackedWidget>

#include "beep.h"

class InterfaceModuleAddPopup : public QDialog {
    Q_OBJECT

  public:
    InterfaceModuleAddPopup(QWidget* parent = nullptr, void* module = nullptr);
    ~InterfaceModuleAddPopup();

  private:
    /* 模块名称 */
    QLineEdit* edit_name;
    /* 模块地址 */

    /* 槽 */
  public slots:
    /* 模块弹窗添加按钮单击事件 */
    void module_popup_btn_add_clicked();
    /* 模块弹窗取消按钮单击事件 */
    void module_popup_btn_cancel_clicked();
    /* 模块弹窗测试按钮单击事件 */
    void module_popup_btn_test_clicked();
    /* 模块弹窗删除按钮单击事件 */
    void module_popup_btn_delete_clicked();
};

class InterfaceModule : public QFrame {
    Q_OBJECT

  public:
    InterfaceModule(QWidget* parent = nullptr);
    ~InterfaceModule();

  private:
    int page_max;   // 最大页
    int page_cur;   // 当前页

    /* 标题 */
    QLabel* title;
    struct {
        /* 模块按钮 */
        QPushButton* btn;
        /* 模块按钮栈 */
        QStackedWidget* stack;
        /* 模块名称 */
        QLabel* name;
        /* 模块地址 */
        QLabel* address;
        /* 模块类型 */
        QLabel* type;
    } module[8];

    /* 左翻页按钮 */
    QPushButton* btn_left_page;
    /* 左翻页按钮栈 */
    QStackedWidget* btn_left_page_stack;
    /* 右翻页按钮 */
    QPushButton* btn_right_page;
    /* 右翻页按钮栈 */
    QStackedWidget* btn_right_page_stack;

    /* 页码显示 */
    QLabel* page_number;
    /* 页面刷新 */
    void page_refresh(int page);
    /* 添加弹窗 */
    InterfaceModuleAddPopup* add_popup;

    void showEvent(QShowEvent* event) override;
    /* 模块弹窗 */
    void module_popup(int mode);

    /* 信号 */
  signals:
    /* 槽 */
  public slots:
    /* 模块按钮点击事件 */
    void btn_module_clicked(int id);
    /* 模块按钮点击事件 */
    void btn_left_page_clicked();
    /* 模块按钮点击事件 */
    void btn_right_page_clicked();
};
