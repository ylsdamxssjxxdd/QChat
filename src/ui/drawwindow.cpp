#include "friendwindow.h"
#include "ui_friendwindow.h"

FriendWindow::FriendWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FriendWindow)
{
    ui->setupUi(this);
}

FriendWindow::~FriendWindow()
{
    delete ui;
}
