#ifndef EXPANDWINDOW_H
#define EXPANDWINDOW_H

#include <QWidget>

namespace Ui {
class ExpandWindow;
}

class ExpandWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ExpandWindow(QWidget *parent = nullptr);
    ~ExpandWindow();

private:
    Ui::ExpandWindow *ui;
};

#endif // EXPANDWINDOW_H
