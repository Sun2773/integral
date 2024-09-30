#pragma once

#include <QMainWindow>
#include <QPushButton>
#include <QDebug>
#include <QGuiApplication>
#include <QScreen>
#include <QFrame>

#include "beep.h"

class InterfaceMain : public QFrame {
    Q_OBJECT

   public:
    InterfaceMain(QWidget* parent = nullptr);
    ~InterfaceMain();

   private:

    /* 信号 */
   signals:
    /* 槽 */
   public slots:
};
