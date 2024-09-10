#include "beep.h"

const int _BEEP_Stop[]                = {0};                             // 停止
const int _BEEP_VeryShort[]           = {5, 0};                          // 超短鸣一次    5ms
const int _BEEP_Short[]               = {100, 0};                        // 短鸣一次      100ms
const int _BEEP_Medium[]              = {500, 0};                        // 中鸣一次      500ms
const int _BEEP_Long[]                = {1000, 0};                       // 长鸣一次      1s
const int _BEEP_DoubleShort[]         = {80, 160, 80, 680, 0};           // 双短鸣一次
const int _BEEP_DoubleShortContinue[] = {80, 160, 80, 680, -1};          // 双短鸣持续
const int _BEEP_Alarm[]               = {80, 50, 80, 50, 80, 600, -1};   // 报警音持续
const int _BEEP_WarningTone_1[]       = {500, 160, 80, 160, 80, 0};      // 提示音1
const int _BEEP_WarningTone_2[]       = {80, 50, 80, 50, 80, 0};         // 提示音2
const int _BEEP_WarningTone_3[]       = {80, 50, 80, 0};                 // 提示音3
const int _BEEP_WarningTone_4[]       = {150, 50, 80, 0};                // 提示音4

const int* BeepVoice[] = {

    _BEEP_Stop,
    _BEEP_VeryShort,
    _BEEP_Short,
    _BEEP_Medium,
    _BEEP_Long,
    _BEEP_DoubleShort,
    _BEEP_DoubleShortContinue,
    _BEEP_Alarm,
    _BEEP_WarningTone_1,
    _BEEP_WarningTone_2,
    _BEEP_WarningTone_3,
    _BEEP_WarningTone_4,
};

Beep::Beep(void) {
    file.setFileName("/sys/devices/platform/leds/leds/beep/brightness");

    wait = new QMutex();
    wait->tryLock();
    start();
}

void Beep::run(void) {
    bool state      = false;
    int  delay      = -1;
    int* task1      = NULL;
    int* task2      = NULL;
    int  task1Index = 0;
    int  task2Index = 0;

    while (1) {
        state = wait->tryLock(delay);
        qDebug() << "蜂鸣器任务运行";
        if (state == true) {
            task1 = (int*) BeepVoice[command];
            if (task1[0] == 0) {
                task1 = 0;
                task2 = 0;
            }
            task1Index = 0;
        }

        if (task1) {
            if (task1[task1Index] == 0) {
                delay = 0;
                task1 = 0;
                off();
            } else if (task1[task1Index] == -1) {
                off();
                delay      = 0;
                task2      = task1;
                task2Index = 0;
                task1      = 0;
            } else {
                if (task1Index % 2)
                    off();
                else
                    on();
                delay = task1[task1Index++];
            }
        } else if (task2) {
            if (task2[task2Index] == 0) {
                delay = 0;
                task2 = 0;
                off();
            } else if (task2[task2Index] == -1) {
                off();
                delay      = 0;
                task2Index = 0;
            } else {
                if (task2Index % 2) {
                    off();
                } else {
                    on();
                }
                delay = task2[task2Index++];
            }
        } else {
            delay = -1;
            off();
        }
    }
}

void Beep::sound(int state) {
    /* 蜂鸣器未就绪，不做任何操作 */
    // if (ready == false) {
    //     return;
    // }
    command = state;
    wait->unlock();
}

void Beep::on(void) {
    if (!(ready = file.exists())) {
        qDebug() << "文件不存在";
        return;
    }
    file.open(QIODevice::ReadWrite);
    file.write("1");
    qDebug() << "蜂鸣器打开";
    file.close();
}

void Beep::off(void) {
    if (!(ready = file.exists())) {
        qDebug() << "文件不存在";
        return;
    }
    file.open(QIODevice::ReadWrite);
    file.write("0");
    qDebug() << "蜂鸣器关闭";
    file.close();
}