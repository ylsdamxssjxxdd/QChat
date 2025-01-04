#ifndef LAN_H
#define LAN_H

#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>
#include <QSet>
#include <QTimer>
#include <QNetworkInterface>
#include <QThreadPool>

class LAN : public QObject
{
    Q_OBJECT

public:
    explicit LAN(QObject* parent = nullptr);
    ~LAN();

    // 开始将此设备暴露给其他局域网
    void startBroadcasting();

    // 开始发现指定网段中的其他局域网设备
    void startDiscovery(const QString& baseAddress);

    // 获取已发现的局域网列表
    QSet<QHostAddress> getDiscoveredLANs() const;

    // 添加新的网段用于发现
    void addNetworkSegmentForDiscovery(const QString& baseAddress);

    void startDiscoveryForAllSegments();

    
private slots:
    // 处理传入的数据报
    void processPendingDatagrams();

    // 向网络广播身份信息
    void broadcastIdentity();

private:
    QUdpSocket* udpSocket;            // 用于通信的UDP套接字
    quint16 broadcastPort;             // 用于广播和接收消息的端口
    QSet<QHostAddress> discoveredLANs; // 已发现的局域网设备集合
    QStringList networkSegments;      // 要发现的网段列表

    // 根据机器的IP地址获取本地网段
    QStringList getLocalNetworkSegments() const;
    
};

#endif // LAN_H
