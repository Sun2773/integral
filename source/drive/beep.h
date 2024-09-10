#pragma once

#include <QDebug>
#include <QFile>
#include <QMutex>
#include <QThread>

class Beep : public QThread {
   public:
    Beep(void);

    void sound(int state);

    void on(void);
    void off(void);

   private:
    void run() override;

    bool ready;

    int command;

    QMutex* wait;

    QFile file;
};
