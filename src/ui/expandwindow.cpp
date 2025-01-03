#include "expandwindow.h"
#include "ui_expandwindow.h"

ExpandWindow::ExpandWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ExpandWindow)
{
    ui->setupUi(this);
}

ExpandWindow::~ExpandWindow()
{
    delete ui;
}
