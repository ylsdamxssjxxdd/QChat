#include "LAN.h"
#include <QNetworkDatagram>
#include <QDebug>
#include <QNetworkInterface>
#include <QThreadPool>
#include <QRunnable>

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
    
    // 使用线程池进行并行化发现
    QThreadPool::globalInstance()->start([this, baseAddress]() {
        for (int i = 1; i <= 255; ++i) {
            QString targetAddress = baseAddress + QString::number(i);
            udpSocket->writeDatagram("Discovery", QHostAddress(targetAddress), broadcastPort);
        }
    });
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
    while (udpSocket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = udpSocket->receiveDatagram();
        QHostAddress senderAddress = datagram.senderAddress();

        // 使用set进行去重，避免重复存储已发现设备
        if (datagram.data() == "LAN-Device" && !discoveredLANs.contains(senderAddress)) {
            discoveredLANs.insert(senderAddress);
            qDebug() << "Discovered LAN device at" << senderAddress.toString();
        }
    }
}

void LAN::broadcastIdentity()
{
    QByteArray broadcastMessage = "LAN-Device"; // 简化消息内容
    udpSocket->writeDatagram(broadcastMessage, QHostAddress::Broadcast, broadcastPort);
    qDebug() << "Broadcasting identity";
}

QStringList LAN::getLocalNetworkSegments() const
{
    QStringList segments;
    // 获取机器的所有网络接口
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    
    for (const QNetworkInterface& interface : interfaces) {
        QList<QNetworkAddressEntry> entries = interface.addressEntries();
        
        for (const QNetworkAddressEntry& entry : entries) {
            // 仅考虑IPv4地址
            if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol) {
                QString ip = entry.ip().toString();

                // 过滤回环地址（127.0.0.1）和APIPA地址（169.254.x.x）
                if (entry.ip().isLoopback() || ip.startsWith("169.254")) {
                    continue;
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
    for (const QString& segment : networkSegments) {
        startDiscovery(segment);
    }
}
