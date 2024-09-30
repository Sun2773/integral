#include "module.h"

Module::Module() {
    // 获取应用程序可执行文件所在的目录
    QString appDirPath = QCoreApplication::applicationDirPath();
    appDirPath += "/../config/BH9160.CB";
    file = new QFile(appDirPath);
    /* 判断文件是否存在 */
    if (file->exists()) {
        /* 以只读的方式打开 */
        file->open(QFile::ReadOnly | QIODevice::Text);
        QByteArray      data = file->readAll();
        QJsonParseError error;
        /* 以字符串的方式保存读出的结果 */
        jsonDoc = QJsonDocument::fromJson(data, &error);
        /* 关闭文件 */
        file->close();
        if (jsonDoc.isNull()) {
            qDebug() << "Invalid JSON data";
        }
        // 检查文档是否包含一个对象
        if (!jsonDoc.isObject()) {
            qDebug() << "JSON document is not an object";
        }
        QJsonObject jsonObject = jsonDoc.object();
        {
            QJsonArray slave = jsonObject["Slave"].toArray();
            {
                QJsonObject slaveObject = slave[0].toObject();
                slaveObject["Address"]  = 10;
                slave[0]                = slaveObject;
            }
            jsonObject["Slave"] = slave;
        }
        jsonDoc.setObject(jsonObject);
        qDebug() << jsonDoc;
    }
}

Module::~Module() {
}

int Module::count() {
    QJsonObject jsonObject = jsonDoc.object();
    QJsonArray  jsonArray  = jsonObject["Slave"].toArray();
    qDebug() << jsonArray.size();
}
void Module::Test() {
}
