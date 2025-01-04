#include <QCoreApplication>
#include <QDebug>
#include "LAN.h"

bool isValidNetworkSegment(const QString& segment) {
    // Validate network segment format (xxx.xxx.xxx.)
    QRegExp regex("^\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.$");
    return regex.exactMatch(segment);
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    LAN lan;

    // Process command line arguments
    for (int i = 1; i < argc; ++i) {
        QString segment = QString::fromLocal8Bit(argv[i]);
        if (isValidNetworkSegment(segment)) {
            lan.addNetworkSegmentForDiscovery(segment);
            qDebug() << "Added network segment:" << segment;
        } else {
            qWarning() << "Invalid network segment format:" << segment;
            qWarning() << "Expected format: xxx.xxx.xxx.";
        }
    }


    return a.exec();
}
