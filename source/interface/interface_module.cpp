#include "interface_module.h"

#include "mainwindow.h"
#include "slave.h"

extern Slave* slaveDrive;
InterfaceModule::InterfaceModule(QWidget* parent)
    : QFrame(parent) {
    _MainWindow->addInterface(this);
    { /* 指定QSS样式 */
        QFile file(":/style/interface_module.qss");
        /* 判断文件是否存在 */
        if (file.exists()) {
            /* 以只读的方式打开 */
            file.open(QFile::ReadOnly);
            /* 以字符串的方式保存读出的结果 */
            QString styleSheet = QLatin1String(file.readAll());
            /* 设置全局样式 */
            this->setStyleSheet(styleSheet);
            /* 关闭文件 */
            file.close();
        }
    }
    { /* 窗口属性栏属性 */
        this->setObjectName("interface_module");
        this->setGeometry(70, 50, parent->size().width() - 70 - 10, parent->size().height() - 50 - 10);
    }
    {
        /* 标题 */
        title = new QLabel("模块管理", this);
        title->resize(150, 30);
        title->move((this->size().width() - title->size().width()) / 2, 5);
        QFont font("Arial", 20);                // 设置字体为 Arial，大小为 20
        title->setFont(font);                   //
        title->setAlignment(Qt::AlignCenter);   // 设置垂直居中对齐
    }
    { /* 添加模块按钮 */
        QSignalMapper* signalMapper = new QSignalMapper();
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 4; j++) {
                module[i * 4 + j].btn = new QPushButton(this);
                module[i * 4 + j].btn->setObjectName("btn_add_module");
                module[i * 4 + j].btn->resize(150, 150);
                module[i * 4 + j].btn->move(45 + 160 * j, 45 + 160 * i);
                {
                    module[i * 4 + j].stack = new QStackedWidget(module[i * 4 + j].btn);

                    { /* 待添加状态展示 */
                        QLabel* bg_add = new QLabel();
                        bg_add->setPixmap(QPixmap(":/picture/btn_add_module.png"));
                        module[i * 4 + j].stack->addWidget(bg_add);
                    }
                    { /* 已填加状态展示 */
                        QLabel* bg_ord = new QLabel();
                        bg_ord->setPixmap(QPixmap(":/picture/btn_module.png"));
                        QLabel*      label;
                        QGridLayout* layout = new QGridLayout(bg_ord);

                        /********** 模块名称展示 **********/
                        label = new QLabel("名称:", bg_ord);
                        // label->setStyleSheet("background-color: yellow;");
                        layout->addWidget(label, 0, 0);
                        module[i * 4 + j].name = new QLabel(bg_ord);
                        // module[i * 4 + j].name->setStyleSheet("background-color: yellow;");
                        layout->addWidget(module[i * 4 + j].name, 1, 0, 1, 2);

                        /********** 模块地址展示 **********/
                        label = new QLabel("地址:", bg_ord);
                        layout->addWidget(label, 2, 0);
                        module[i * 4 + j].address = new QLabel(bg_ord);
                        layout->addWidget(module[i * 4 + j].address, 2, 1);

                        /********** 模块类型展示 **********/
                        label = new QLabel("类型:", bg_ord);
                        layout->addWidget(label, 3, 0);
                        module[i * 4 + j].type = new QLabel(bg_ord);
                        layout->addWidget(module[i * 4 + j].type, 3, 1);

                        /* 添加状态到栈中 */
                        module[i * 4 + j].stack->addWidget(bg_ord);
                    }
                }
                /* 将按钮映射到一个整数 */
                signalMapper->setMapping(module[i * 4 + j].btn, i * 4 + j);
                /* 为按钮连接槽函数 */
                connect(module[i * 4 + j].btn,
                        &QPushButton::clicked,
                        signalMapper,
                        QOverload<>::of(&QSignalMapper::map));
                /* 按钮默认隐藏 */
                module[i * 4 + j].btn->hide();
            }
        }
        connect(signalMapper, QOverload<int>::of(&QSignalMapper::mapped), this, &InterfaceModule::btn_module_clicked);
    }
    {
        btn_left_page = new QPushButton(this);
        btn_left_page->setObjectName("btn_left_page");
        btn_left_page->resize(20, 60);
        btn_left_page->move(10, (this->size().height() - btn_left_page->size().height()) / 2);
        {
            btn_left_page_stack = new QStackedWidget(btn_left_page);
            { /* 无效状态展示 */
                QLabel* invalid = new QLabel();
                invalid->setPixmap(QPixmap(":/picture/btn_left_page_invalid.png"));
                btn_left_page_stack->addWidget(invalid);
            }
            { /* 有效状态展示 */
                QLabel* valid = new QLabel();
                valid->setPixmap(QPixmap(":/picture/btn_left_page_valid.png"));
                btn_left_page_stack->addWidget(valid);
            }
        }
        connect(btn_left_page, &QPushButton::clicked, this, &InterfaceModule::btn_left_page_clicked);
    }
    {
        btn_right_page = new QPushButton(this);
        btn_right_page->setObjectName("btn_right_page");
        btn_right_page->resize(20, 60);
        btn_right_page->move(this->size().width() - 30, (this->size().height() - btn_right_page->size().height()) / 2);
        {
            btn_right_page_stack = new QStackedWidget(btn_right_page);
            { /* 无效状态展示 */
                QLabel* invalid = new QLabel();
                invalid->setPixmap(QPixmap(":/picture/btn_right_page_invalid.png"));
                btn_right_page_stack->addWidget(invalid);
            }
            { /* 有效状态展示 */
                QLabel* valid = new QLabel();
                valid->setPixmap(QPixmap(":/picture/btn_right_page_valid.png"));
                btn_right_page_stack->addWidget(valid);
            }
        }
        connect(btn_right_page, &QPushButton::clicked, this, &InterfaceModule::btn_right_page_clicked);
    }
    {
        page_number = new QLabel(this);
        page_number->resize(100, 30);
        page_number->move((this->size().width() - page_number->size().width()) / 2,
                          (this->size().height() - 30 - 20));
        title->setAlignment(Qt::AlignCenter);   // 设置垂直居中对齐
    }
}

InterfaceModule::~InterfaceModule() {
}

void InterfaceModule::showEvent(QShowEvent* event) {
    // 调用基类的 showEvent 方法
    QWidget::showEvent(event);
    page_refresh(1);
}

void InterfaceModule::btn_module_clicked(int id) {
    if (module[id].stack->currentIndex() != 0) {
        return;
    }
    // int currentIndex = module[id].stack->currentIndex();
    // module[id].stack->setCurrentIndex((currentIndex + 1) % module[id].stack->count());
    // SlaveDrive* drive = new SlaveDrive();
    // drive->Slave      = 0x0A;               // 设备地址
    // drive->Name       = "测试模块";         // 设备名称
    // drive->RegSize    = 10;                 // 寄存器大小
    // drive->RegStart   = 0;                  // 寄存器起始地址
    // drive->Reg        = new uint16_t[10];   // 寄存器指针
    // drive->SlaveType  = 1;                  // 设备类型
    // drive->Outtime    = 20;                 // 超时计数
    // slaveDrive->drive_add(drive);
    module_popup(0);
    qDebug() << "模块按钮" << id << "触发";
}

void InterfaceModule::btn_left_page_clicked() {
    if (btn_left_page_stack->currentIndex() == 0) {
        return;
    }
    page_refresh(page_cur - 1);
}

void InterfaceModule::btn_right_page_clicked() {
    if (btn_right_page_stack->currentIndex() == 0) {
        return;
    }
    page_refresh(page_cur + 1);
}

void InterfaceModule::page_refresh(int page) {
    int drive_cnt = slaveDrive->drive_count();
    /* 计算一共多少页 */
    page_max = (drive_cnt + 1) / 8 + (((drive_cnt + 1) % 8) != 0);
    if (page > page_max) {
        page_cur = page_max;
    } else if (page < 1) {
        page_cur = 1;
    } else {
        page_cur = page;
    }
    for (int i = 0; i < 8; i++) {
        if ((page_cur - 1) * 8 + i < drive_cnt) {
            module[i % 8].btn->show();
            module[i % 8].stack->setCurrentIndex(1);
            // module[i % 8].name->setText(slaveDrive->drive_get((page_cur - 1) * 8 + i)->Name);
        } else if ((page_cur - 1) * 8 + i == drive_cnt) {
            module[i % 8].btn->show();
            module[i % 8].stack->setCurrentIndex(0);
        } else {
            module[i % 8].btn->hide();
        }
    }
    if (page_cur == 1) {
        btn_left_page_stack->setCurrentIndex(0);
    } else {
        btn_left_page_stack->setCurrentIndex(1);
    }
    if (page_cur == page_max) {
        btn_right_page_stack->setCurrentIndex(0);
    } else {
        btn_right_page_stack->setCurrentIndex(1);
    }
    page_number->setText(QString::number(page_cur) + "/" + QString::number(page_max));
}

InterfaceModuleAddPopup ::InterfaceModuleAddPopup(QWidget* parent, void* module)
    : QDialog(parent) {
    this->resize(400, 300);                                              // 设置窗口大小
    this->move((parent->size().width() - this->size().width()) / 2,      // 设定弹出位置
               (parent->size().height() - this->size().height()) / 2);   //

    { /* 弹窗标题栏 */
        QLabel* title = new QLabel("添加模块", this);
        title->setObjectName("module_add_popup_title");
        title->resize(this->size().width(), 50);
        title->move(0, 0);
        title->setAlignment(Qt::AlignCenter);
        title->setFont(QFont("Arial", 18));

        { /* 取消按钮 */
            QPushButton* btn_cancel = new QPushButton(title);
            btn_cancel->resize(30, 30);
            btn_cancel->move(title->size().width() - btn_cancel->size().width() - 10,
                             10);
            connect(btn_cancel,
                    &QPushButton::clicked,
                    this,
                    &InterfaceModuleAddPopup::module_popup_btn_cancel_clicked);
        }
    }
    {
        QFrame* add_popup_frame = new QFrame(this);
        add_popup_frame->setObjectName("module_add_popup_frame");
        add_popup_frame->resize(this->size().width(), this->size().height() - 50);
        add_popup_frame->move(0, 50);

        { /* 添加测试按钮 */
            QPushButton* btn_add_test = new QPushButton(add_popup_frame);
            btn_add_test->resize(80, 40);
            btn_add_test->move((add_popup_frame->size().width() - btn_add_test->size().width()) / 2,
                               (add_popup_frame->size().height() - btn_add_test->size().height()) - 10);
            if (module == nullptr) {
                btn_add_test->setText("添加");
                connect(btn_add_test,
                        &QPushButton::clicked,
                        this,
                        &InterfaceModuleAddPopup::module_popup_btn_add_clicked);
            } else {
                btn_add_test->setText("测试");
                connect(btn_add_test,
                        &QPushButton::clicked,
                        this,
                        &InterfaceModuleAddPopup::module_popup_btn_test_clicked);
            }
        }

        {
            QLabel* label_name = new QLabel("模块名称:", add_popup_frame);
            label_name->move(20, 15);
            label_name->setFont(QFont("Arial", 16));
        }
        {
            edit_name = new QLineEdit(add_popup_frame);
            edit_name->resize(200, 30);
            edit_name->move(100, 15);
            edit_name->setFont(QFont("Arial", 16));
            edit_name->setText("请输入模块名称");
        }
        {
            QLabel* label_address = new QLabel("模块地址:", add_popup_frame);
            label_address->move(20, 60);
            label_address->setFont(QFont("Arial", 16));
        }
        {
            QLabel* label_type = new QLabel("模块类型:", add_popup_frame);
            label_type->move(20, 105);
            label_type->setFont(QFont("Arial", 16));
        }
        if (module == nullptr) {
            QLabel* label_version = new QLabel("模块版本:", add_popup_frame);
            label_version->move(20, 150);
            label_version->setFont(QFont("Arial", 16));
        }
    }
}

InterfaceModuleAddPopup ::~InterfaceModuleAddPopup() {
}

void InterfaceModule::module_popup(int mode) {
    add_popup = new InterfaceModuleAddPopup(this, nullptr);               // 弹出窗口
    add_popup->setAttribute(Qt::WA_TranslucentBackground);                // 设置窗口背景为透明
    add_popup->setWindowModality(Qt::ApplicationModal);                   // 设置为应用模态模式
    add_popup->setWindowFlags(windowFlags() | Qt::FramelessWindowHint);   // 移除默认的窗口边框

    add_popup->exec();

    delete add_popup;   // 移除弹窗
    add_popup = nullptr;
}

void InterfaceModuleAddPopup::module_popup_btn_add_clicked() {
    this->close();
}

void InterfaceModuleAddPopup::module_popup_btn_cancel_clicked() {
    this->close();
}

void InterfaceModuleAddPopup::module_popup_btn_delete_clicked() {
}

void InterfaceModuleAddPopup::module_popup_btn_test_clicked() {
}