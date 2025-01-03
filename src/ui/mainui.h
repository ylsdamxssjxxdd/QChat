#ifndef MAINUI_H
#define MAINUI_H

#include <QWidget>
#include <QDebug>
#include <QFile>
#include <QButtonGroup>

#include "chatwindow.h"
#include "friendwindow.h"
#include "expandwindow.h"
#include "settingswindow.h"


namespace Ui {
class MainUi;
}

class MainUi : public QWidget
{
    Q_OBJECT

public:
    explicit MainUi(QWidget *parent = nullptr);
    ~MainUi();

private:
    Ui::MainUi *ui;

    QButtonGroup btnGroup;
    ChatWindow chatWindow;
    FriendWindow friendWindow;
    ExpandWindow expandWindow;
    SettingsWindow settingsWindow;

};

#endif // MAINUI_H
