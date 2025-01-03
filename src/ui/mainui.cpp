#include "mainui.h"
#include "ui_mainui.h"

MainUi::MainUi(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainUi)
{
    ui->setupUi(this);

    // add child widget
    ui->stackedWidget->addWidget(&chatWindow);
    ui->stackedWidget->addWidget(&friendWindow);
    ui->stackedWidget->addWidget(&expandWindow);
    ui->stackedWidget->addWidget(&settingsWindow);

    // add child button
    btnGroup.addButton(ui->btnChat,0);
    btnGroup.addButton(ui->btnFriend,1);
    btnGroup.addButton(ui->btnExpand,2);
    btnGroup.addButton(ui->btnSettings,3);

    // connet button and widget
    connect(&btnGroup,QOverload<int>::of(&QButtonGroup::idClicked),ui->stackedWidget,&QStackedWidget::setCurrentIndex);

    // set default child widget
    btnGroup.button(0)->setChecked(true);
    ui->stackedWidget->setCurrentIndex(0);

    // apply qss
    QFile file(":/qss/EVA_default.qss");
    file.open(QFile::ReadOnly);
    QString stylesheet = tr(file.readAll());
    this->setStyleSheet(stylesheet);

    qDebug()<<"mainui init over";
}

MainUi::~MainUi()
{
    delete ui;
}
