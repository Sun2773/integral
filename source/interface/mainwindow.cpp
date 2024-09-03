#include "mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
    this->resize(800, 480);

    pushBUtton = new QPushButton(this);

    pushBUtton->setText("这是一个按钮");
    pushBUtton->resize(200, 100);
}

MainWindow::~MainWindow() {
}
