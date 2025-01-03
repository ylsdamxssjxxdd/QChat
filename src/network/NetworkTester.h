#ifndef NETWORKTESTER_H
#define NETWORKTESTER_H

#include "NetworkManager.h"
#include <QObject>
#include <QTimer>
#include <QDebug>

/**
 * @brief 网络测试类
 * 
 * 用于测试NetworkManager的功能，包括消息传输、文件传输和服务发现
 */
class NetworkTester : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param manager 网络管理器实例
     * @param isServer 是否作为服务器运行
     * @param parent 父对象指针
     */
    explicit NetworkTester(NetworkManager* manager, QObject *parent = nullptr);

    /**
     * @brief 启动测试
     */
    void start();

signals:
    /**
     * @brief 测试完成信号
     */
    void testCompleted();

private slots:
    /**
     * @brief 处理接收到的消息
     * @param message 接收到的消息内容
     */
    void onMessageReceived(const QString& message);

    /**
     * @brief 处理接收到的文件
     * @param fileName 接收到的文件路径
     */
    void onFileReceived(const QString& fileName);

    /**
     * @brief 处理发现的新用户
     * @param userInfo 新用户信息
     */
    void onUserDiscovered(const QString& userInfo);

    /**
     * @brief 广播存在信息
     */
    void broadcastPresence();

    /**
     * @brief 发送测试消息
     */
    void sendTestMessage();

    /**
     * @brief 发送测试文件
     */
    void sendTestFile();

private:
    NetworkManager* manager;  ///< 网络管理器实例
    bool isServer;            ///< 是否作为服务器运行
};

#endif // NETWORKTESTER_H
