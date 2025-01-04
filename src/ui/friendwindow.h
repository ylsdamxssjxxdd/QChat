#ifndef FriendWINDOW_H
#define FriendWINDOW_H

#include <QWidget>

namespace Ui {
class FriendWindow;
}

class FriendWindow : public QWidget
{
    Q_OBJECT

public:
    explicit FriendWindow(QWidget *parent = nullptr);
    ~FriendWindow();


private:
    Ui::FriendWindow *ui;
};

#endif // FriendWINDOW_H
