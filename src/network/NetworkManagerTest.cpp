#include "NetworkManager.h"
#include "NetworkTester.h"
#include <QCoreApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    // 设置默认端口
    quint16 basePort = 12345;
    
    // 如果传入参数，使用指定端口
    if(argc > 1) {
        bool ok;
        quint16 port = QString(argv[1]).toUShort(&ok);
        if(ok) {
            basePort = port;
        }
    }

    NetworkManager manager;
    if(!manager.startServer(basePort)) {
        qCritical() << "Failed to start server on port" << basePort;
        return 1;
    }

    NetworkTester tester(&manager);
    
    QTimer::singleShot(0, &tester, &NetworkTester::start);

    return app.exec();
}
