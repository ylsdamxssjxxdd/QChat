#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QHostAddress>
#include <QMap>
#include <QDataStream>
#include <QStandardPaths>
#include <QHostInfo>

/**
 * @brief 消息类型枚举
 * 
 * 定义网络传输的消息类型，目前支持文本和文件两种类型
 */
enum class MessageType : quint8 {
    Text = 0,   ///< 文本消息
    File = 1    ///< 文件传输
};

/**
 * @brief 网络管理类
 * 
 * 负责管理网络连接、消息传输和文件传输功能。
 * 使用TCP进行可靠数据传输，UDP进行服务发现。
 */
class NetworkManager : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param parent 父对象指针
     */
    explicit NetworkManager(QObject *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~NetworkManager();

    /**
     * @brief 启动服务器
     * @param basePort 起始端口号
     * @return 成功返回true，失败返回false
     */
    bool startServer(quint16 basePort);

    /**
     * @brief 连接到对等节点
     * @param ip 目标IP地址
     * @param port 目标端口号
     */
    void connectToPeer(const QString &ip, quint16 port);

    /**
     * @brief 发送文本消息
     * @param message 要发送的消息内容
     */
    void sendMessage(const QString &message);

    /**
     * @brief 发送文件
     * @param filePath 要发送的文件路径
     */
    void sendFile(const QString &filePath);

    /**
     * @brief 广播存在信息
     * 
     * 使用组播方式广播当前用户的存在信息
     * @param groupAddress 组播地址
     */
    void broadcastPresence(const QHostAddress &groupAddress = QHostAddress("239.255.43.21"));

    /**
     * @brief 加入组播组
     * @param groupAddress 组播地址
     */
    void joinMulticastGroup(const QHostAddress &groupAddress);

    /**
     * @brief 离开组播组 
     * @param groupAddress 组播地址
     */
    void leaveMulticastGroup(const QHostAddress &groupAddress);

signals:
    /**
     * @brief 接收到新消息信号
     * @param message 接收到的消息内容
     */
    void newMessageReceived(const QString &message);

    /**
     * @brief 接收到新文件信号
     * @param fileName 接收到的文件保存路径
     */
    void newFileReceived(const QString &fileName);

    /**
     * @brief 发现新用户信号
     * @param userInfo 新用户信息
     */
    void newUserDiscovered(const QString &userInfo);

private slots:
    /**
     * @brief 处理新连接
     */
    void onNewConnection();

    /**
     * @brief 处理数据接收
     */
    void onReadyRead();

    /**
     * @brief 处理连接断开
     */
    void onDisconnected();

private:
    QTcpServer *tcpServer;  ///< TCP服务器实例
    QUdpSocket *udpSocket;  ///< UDP套接字实例
    QMap<QTcpSocket*, QString> connections; ///< 当前连接映射表
    
    /**
     * @brief 处理接收到的数据
     * @param socket 来源套接字
     * @param data 接收到的数据
     */
    void processIncomingData(QTcpSocket *socket, const QByteArray &data);

    /**
     * @brief 处理文件传输
     * @param socket 来源套接字
     * @param data 接收到的文件数据
     */
    void handleFileTransfer(QTcpSocket *socket, const QByteArray &data);
    
    /**
     * @brief 处理UDP数据报接收
     */
    void onUdpReadyRead();
};

#endif // NETWORKMANAGER_H
