#include "LAN.h"
#include <QNetworkDatagram>
#include <QDebug>
#include <QNetworkInterface>

LAN::LAN(QObject* parent)
    : QObject(parent), udpSocket(new QUdpSocket(this)), broadcastPort(45454)
{
    // 将readyRead信号连接到处理传入数据报的槽
    connect(udpSocket, &QUdpSocket::readyRead, this, &LAN::processPendingDatagrams);

    // 使用本地网段初始化网段列表
    networkSegments = getLocalNetworkSegments();

    // 设置定时器定期发送广播消息
    QTimer* broadcastTimer = new QTimer(this);
    connect(broadcastTimer, &QTimer::timeout, this, &LAN::broadcastIdentity);
    broadcastTimer->start(2000); // 每2秒广播一次

    // 开始广播和发现
    startBroadcasting();
    startDiscoveryForAllSegments();
}

LAN::~LAN()
{
    // 不需要手动删除udpSocket，因为它是该对象的子对象，会自动删除
}

void LAN::startBroadcasting()
{
    // 将UDP套接字绑定到广播端口和地址
    udpSocket->bind(QHostAddress::AnyIPv4, broadcastPort, QUdpSocket::ShareAddress);
    broadcastIdentity(); // 立即发送第一次广播
}

void LAN::startDiscovery(const QString& baseAddress)
{
    qDebug() << "Starting discovery for segment:" << baseAddress;
    for (int i = 1; i <= 255; ++i)
    {
        QString targetAddress = baseAddress + QString::number(i);
        udpSocket->writeDatagram("Discovery", QHostAddress(targetAddress), broadcastPort);
        // qDebug() << "Sending discovery to" << targetAddress;
    }
}

QSet<QHostAddress> LAN::getDiscoveredLANs() const
{
    return discoveredLANs;
}

void LAN::addNetworkSegmentForDiscovery(const QString& baseAddress)
{
    // 向列表添加新网段并开始发现该网段
    if (!networkSegments.contains(baseAddress)) {
        networkSegments.append(baseAddress);
        startDiscovery(baseAddress);
        qDebug() << "Added and started discovery for new segment:" << baseAddress;
    }
}

void LAN::processPendingDatagrams()
{
    while (udpSocket->hasPendingDatagrams())
    {
        QNetworkDatagram datagram = udpSocket->receiveDatagram();
        QHostAddress senderAddress = datagram.senderAddress();

        if (datagram.data() == "LAN-Device")
        {
        // 如果消息是"LAN-Device"消息，存储发送者地址
            discoveredLANs.insert(senderAddress);
            qDebug() << "Discovered LAN device at" << senderAddress.toString();
        }
    }
}

void LAN::broadcastIdentity()
{
    QByteArray broadcastMessage = "LAN-Device"; // 消息内容可以自定义
    udpSocket->writeDatagram(broadcastMessage, QHostAddress::Broadcast, broadcastPort);
    qDebug() << "Broadcasting identity";
}

QStringList LAN::getLocalNetworkSegments() const {
    QStringList segments;
    // 获取机器的所有网络接口
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    
    for (const QNetworkInterface &interface : interfaces) {
        QList<QNetworkAddressEntry> entries = interface.addressEntries();
        
        for (const QNetworkAddressEntry &entry : entries) {
            // 仅考虑IPv4地址
            if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol) {
                QString ip = entry.ip().toString();

                // 过滤回环地址
                if (entry.ip().isLoopback()) {
                    continue; // 跳过回环地址
                }
                
                // 过滤 APIPA 地址 (169.254.x.x)
                if (entry.ip().toString().startsWith("169.254")) {
                    continue; // 跳过 APIPA 地址
                }
           
                // 获取网段
                QString networkSegment = ip.left(ip.lastIndexOf('.') + 1);
                qDebug() << "Local ip:" << networkSegment;
                segments.append(networkSegment);
            }
        }
    }
    return segments;
}

void LAN::startDiscoveryForAllSegments()
{
    for (const QString& segment : networkSegments)
    {
        startDiscovery(segment);
    }
}
