#pragma once

#include <QMainWindow>
#include <QPushButton>
#include <QDebug>
#include "beep.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

   private:
    /*  */
    QPushButton* pushButton;
    Beep* beep;
    /* 信号 */
   signals:
    void pushButtonSignal();
    /* 槽 */
   public slots:
    void pushButtonClicked();
};
