#include "NetworkManager.h"
#include <QFileInfo>
#include <QDateTime>
#include <QNetworkInterface>

/**
 * @brief 构造函数
 * @param parent 父对象指针
 */
NetworkManager::NetworkManager(QObject *parent) : QObject(parent)
{
    tcpServer = new QTcpServer(this);
    udpSocket = new QUdpSocket(this);
    
    // 连接新连接信号
    connect(tcpServer, &QTcpServer::newConnection, this, &NetworkManager::onNewConnection);
}

/**
 * @brief 启动服务器
 * @param port 监听端口号
 */
bool NetworkManager::startServer(quint16 basePort)
{
    // 如果已经监听，直接返回成功
    if(tcpServer->isListening()) {
        return true;
    }

    // 尝试在basePort到basePort+10范围内寻找可用端口
    for(quint16 port = basePort; port <= basePort + 10; ++port) {
        if(!tcpServer->isListening() && tcpServer->listen(QHostAddress::Any, port)) {
            // 启动UDP广播
            // Bind to all available interfaces
            QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
            int boundCount = 0;
            foreach (const QNetworkInterface &interface, interfaces) {
                if (interface.flags().testFlag(QNetworkInterface::IsUp) && 
                    !interface.flags().testFlag(QNetworkInterface::IsLoopBack)) {
                    qDebug() << "Attempting to bind to interface:" << interface.name();
                    if (udpSocket->bind(QHostAddress::AnyIPv4, port + 1, 
                                      QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint)) {
                        qDebug() << "Successfully bound to interface:" << interface.name();
                        boundCount++;
                    } else {
                        qWarning() << "Failed to bind to interface" << interface.name() 
                                 << ":" << udpSocket->errorString();
                    }
                }
            }
            if (boundCount > 0) {
                qDebug() << "Server started on port" << port;
                qDebug() << "UDP bound to port" << udpSocket->localPort();
                qDebug() << "UDP socket state:" << udpSocket->state();
                qDebug() << "UDP socket error:" << udpSocket->errorString();
                return true;
            }
            qWarning() << "Failed to bind UDP socket:" << udpSocket->errorString();
            tcpServer->close();
        }
    }
    qWarning() << "Failed to start server: no available port found";
    return false;
}

/**
 * @brief 连接到对等节点
 * @param ip 目标IP地址
 * @param port 目标端口号
 */
void NetworkManager::connectToPeer(const QString &ip, quint16 port)
{
    QTcpSocket *socket = new QTcpSocket(this);
    socket->connectToHost(ip, port);
    
    // 连接数据接收和断开连接信号
    connect(socket, &QTcpSocket::readyRead, this, &NetworkManager::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &NetworkManager::onDisconnected);
    
    // 将新连接加入连接映射表
    connections.insert(socket, ip);

    // 客户端也绑定UDP端口用于接收广播
    if(!udpSocket->bind(QHostAddress::Any, port + 1, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint)) {
        qWarning() << "Failed to bind UDP socket:" << udpSocket->errorString();
    } else {
        qDebug() << "UDP receiver bound to port" << udpSocket->localPort();
    }
    connect(udpSocket, &QUdpSocket::readyRead, this, &NetworkManager::onUdpReadyRead);
}

/**
 * @brief 发送文本消息
 * @param message 要发送的消息内容
 */
void NetworkManager::sendMessage(const QString &message)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    // 写入消息头：消息长度（占位）、消息类型、消息内容
    stream << quint32(0) << quint8(MessageType::Text) << message;
    
    // 回写实际消息长度
    stream.device()->seek(0);
    stream << quint32(data.size() - sizeof(quint32));
    
    // 向所有连接发送消息
    for(auto socket : connections.keys()) {
        socket->write(data);
    }
}

/**
 * @brief 发送文件
 * @param filePath 要发送的文件路径
 */
void NetworkManager::sendFile(const QString &filePath)
{
    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open file:" << file.errorString();
        return;
    }
    
    // 读取文件内容
    QByteArray fileData = file.readAll();
    QFileInfo fileInfo(filePath);
    
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    // 写入文件头：消息长度（占位）、消息类型、文件名、文件内容
    stream << quint32(0) << quint8(MessageType::File)
         << fileInfo.fileName() << fileData;
    
    // 回写实际消息长度
    stream.device()->seek(0);
    stream << quint32(data.size() - sizeof(quint32));
    
    // 向所有连接发送文件
    for(auto socket : connections.keys()) {
        socket->write(data);
    }
}

    /**
     * @brief 加入组播组
     * @param groupAddress 组播地址
     */
    void NetworkManager::joinMulticastGroup(const QHostAddress &groupAddress)
    {
        // 检查网络接口
        QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
        foreach (const QNetworkInterface &interface, interfaces) {
            if (interface.flags().testFlag(QNetworkInterface::IsUp) && 
                !interface.flags().testFlag(QNetworkInterface::IsLoopBack)) {
                qDebug() << "Trying to join multicast group on interface:" << interface.name();
                
                // 尝试加入组播组
                if (udpSocket->joinMulticastGroup(groupAddress, interface)) {
                    qDebug() << "Successfully joined multicast group:" << groupAddress.toString()
                             << "on interface:" << interface.name();
                    return;
                } else {
                    qWarning() << "Failed to join multicast group on interface" << interface.name()
                             << "Error:" << udpSocket->errorString();
                }
            }
        }
        
        // 如果所有接口都失败，尝试默认加入
        if (!udpSocket->joinMulticastGroup(groupAddress)) {
            qWarning() << "Failed to join multicast group:" << groupAddress.toString()
                     << "Error:" << udpSocket->errorString();
        } else {
            qDebug() << "Successfully joined multicast group:" << groupAddress.toString();
        }
    }

    /**
     * @brief 离开组播组
     * @param groupAddress 组播地址
     */
    void NetworkManager::leaveMulticastGroup(const QHostAddress &groupAddress)
    {
        if (!udpSocket->leaveMulticastGroup(groupAddress)) {
            qWarning() << "Failed to leave multicast group:" << groupAddress.toString()
                     << "Error:" << udpSocket->errorString();
        } else {
            qDebug() << "Successfully left multicast group:" << groupAddress.toString();
        }
    }

    /**
     * @brief 广播存在信息
     * 
     * 使用组播方式广播当前用户的存在信息
     * @param groupAddress 组播地址
     */
    void NetworkManager::broadcastPresence(const QHostAddress &groupAddress)
    {
        QByteArray datagram;
        QDataStream out(&datagram, QIODevice::WriteOnly);
        out << QDateTime::currentDateTime().toString()
            << QHostInfo::localHostName();

        // 加入组播组
        joinMulticastGroup(groupAddress);

        // 发送组播数据报
        qint64 bytesSent = udpSocket->writeDatagram(datagram, groupAddress, udpSocket->localPort());
        qDebug() << "Multicast presence datagram sent to" << groupAddress.toString()
                 << ":" << bytesSent << "bytes";
        if (bytesSent == -1) {
            qDebug() << "Multicast send error:" << udpSocket->errorString();
        }
    }

/**
 * @brief 处理新连接
 */
void NetworkManager::onNewConnection()
{
    QTcpSocket *socket = tcpServer->nextPendingConnection();
    connect(socket, &QTcpSocket::readyRead, this, &NetworkManager::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &NetworkManager::onDisconnected);
    
    // 将新连接加入连接映射表
    connections.insert(socket, socket->peerAddress().toString());
}

/**
 * @brief 处理数据接收
 */
void NetworkManager::onReadyRead()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if(!socket) return;
    
    // 读取所有可用数据
    QByteArray data = socket->readAll();
    processIncomingData(socket, data);
}

/**
 * @brief 处理连接断开
 */
void NetworkManager::onDisconnected()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if(socket) {
        // 从连接映射表中移除并删除套接字
        connections.remove(socket);
        socket->deleteLater();
    }
}

/**
 * @brief 处理接收到的数据
 * @param socket 来源套接字
 * @param data 接收到的数据
 */
void NetworkManager::processIncomingData(QTcpSocket *socket, const QByteArray &data)
{
    QDataStream in(data);
    quint32 blockSize;
    quint8 messageType;
    
    // 读取消息头和类型
    in >> blockSize >> messageType;
    
    switch(static_cast<int>(messageType)) {
    case static_cast<int>(MessageType::Text): {
        QString message;
        in >> message;
        emit newMessageReceived(message);
        break;
    }
    case static_cast<int>(MessageType::File):
        handleFileTransfer(socket, data);
        break;
    default:
        qWarning() << "Unknown message type:" << messageType;
    }
}

/**
 * @brief 处理文件传输
 * @param socket 来源套接字
 * @param data 接收到的文件数据
 */
void NetworkManager::handleFileTransfer(QTcpSocket *socket, const QByteArray &data)
{
    QDataStream in(data);
    quint32 blockSize;
    quint8 messageType;
    QString fileName;
    QByteArray fileData;
    
    // 读取文件头信息
    in >> blockSize >> messageType >> fileName >> fileData;
    
    // 将文件保存到下载目录
    QString savePath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation) + "/" + fileName;
    QFile file(savePath);
    if(file.open(QIODevice::WriteOnly)) {
        file.write(fileData);
        file.close();
        emit newFileReceived(savePath);
    } else {
        qWarning() << "Failed to save file:" << file.errorString();
    }
}

/**
 * @brief 析构函数
 * 
 * 关闭所有连接并释放资源
 */
NetworkManager::~NetworkManager()
{
    // 关闭所有TCP连接
    for(auto socket : connections.keys()) {
        socket->disconnectFromHost();
        socket->deleteLater();
    }
    // 关闭TCP服务器和UDP套接字
    tcpServer->close();
    udpSocket->close();
}

/**
 * @brief 处理UDP数据报接收
 */
void NetworkManager::onUdpReadyRead()
{
    // Check socket state and errors
    if (udpSocket->state() != QUdpSocket::BoundState) {
        qWarning() << "UDP socket not bound, current state:" << udpSocket->state();
        return;
    }
    
    if (udpSocket->error() != QUdpSocket::UnknownSocketError) {
        qWarning() << "UDP socket error:" << udpSocket->errorString();
        return;
    }

    qDebug() << "UDP socket has pending datagrams:" << udpSocket->hasPendingDatagrams();
    qDebug() << "UDP socket state:" << udpSocket->state();
    qDebug() << "UDP socket error:" << udpSocket->errorString();
    
    // Process all pending datagrams
    int receivedCount = 0;
    while(udpSocket->hasPendingDatagrams()) {
        try {
            QByteArray datagram;
            datagram.resize(udpSocket->pendingDatagramSize());
            QHostAddress sender;
            quint16 senderPort;

            // Read UDP datagram
            qint64 bytesRead = udpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
            if (bytesRead == -1) {
                qWarning() << "Failed to read datagram from" << sender.toString() 
                         << ":" << udpSocket->errorString();
                continue;
            }

            qDebug() << "Read UDP datagram from" << sender.toString() << ":" << bytesRead << "bytes";

            // Parse datagram content
            QDataStream in(datagram);
            QString timestamp, hostname;
            in >> timestamp >> hostname;

            // Validate received data
            if (timestamp.isEmpty() || hostname.isEmpty()) {
                qWarning() << "Invalid datagram received from" << sender.toString();
                continue;
            }

            // Trigger new user discovery
            QString userInfo = QString("%1 (%2)").arg(hostname).arg(sender.toString());
            qDebug() << "NetworkManager emitting newUserDiscovered for:" << userInfo;
            emit newUserDiscovered(userInfo);
            receivedCount++;
        } catch (const std::exception &e) {
            qWarning() << "Exception while processing UDP datagram:" << e.what();
        }
    }
    
    if (receivedCount == 0) {
        qDebug() << "No valid UDP datagrams received despite broadcast being sent";
    } else {
        qDebug() << "Successfully processed" << receivedCount << "UDP datagrams";
    }
}
