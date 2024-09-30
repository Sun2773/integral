#pragma once

#include <QByteArray>
#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTextCodec>

//class ModuleInfo{
  
  
//  QString name;
//  QString version;

//}


class Module {
  public:
    Module();
    ~Module();

    int count();
    void Test();

  private:
    QFile*        file;
    QJsonDocument jsonDoc;
};
