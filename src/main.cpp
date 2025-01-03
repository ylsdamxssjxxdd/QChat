#include <QDebug>
#include <QApplication>
#include <QCoreApplication>
#include <QGuiApplication>
#include <QStyleFactory>

#include "ui/mainui.h"

int main(int argc, char* argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);                                        //自适应缩放
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);  //适配非整数倍缩放
    QApplication a(argc, argv);                                                                               //事件实例

    MainUi mainui;       //窗口实例
    mainui.show();

    return a.exec();  //进入事件循环
}