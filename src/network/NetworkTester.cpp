#include "NetworkTester.h"
#include <QFile>
#include <QCoreApplication>

/**
 * @brief 构造函数
 * @param manager 网络管理器实例
 * @param isServer 是否作为服务器运行
 * @param parent 父对象指针
 */
NetworkTester::NetworkTester(NetworkManager* manager, QObject *parent)
    : QObject(parent), manager(manager)
{
    // 连接网络管理器的信号
    connect(manager, &NetworkManager::newMessageReceived,
            this, &NetworkTester::onMessageReceived);
    connect(manager, &NetworkManager::newFileReceived,
            this, &NetworkTester::onFileReceived);
    bool connected = connect(manager, &NetworkManager::newUserDiscovered,
            this, &NetworkTester::onUserDiscovered);
    qDebug() << "newUserDiscovered signal connection status:" << connected;
}

/**
 * @brief 启动测试
 */
void NetworkTester::start()
{
    // 启动服务器
    manager->startServer(12345);
    qDebug() << "Server started on port 12345";
    
    // 创建定时器
    QTimer* broadcastTimer = new QTimer(this);
    QTimer* messageTimer = new QTimer(this);
    QTimer* fileTimer = new QTimer(this);

    // 配置定时器
    broadcastTimer->setInterval(5000);  // 每5秒广播一次
    messageTimer->setInterval(10000);   // 每10秒发送一次消息
    fileTimer->setInterval(20000);      // 每20秒发送一次文件

    // 连接定时器信号
    connect(broadcastTimer, &QTimer::timeout, this, &NetworkTester::broadcastPresence);
    connect(messageTimer, &QTimer::timeout, this, &NetworkTester::sendTestMessage);
    connect(fileTimer, &QTimer::timeout, this, &NetworkTester::sendTestFile);

    // 启动定时器
    broadcastTimer->start();
    messageTimer->start();
    fileTimer->start();

    // 立即发送第一次消息和文件
    QTimer::singleShot(1000, this, &NetworkTester::sendTestMessage);
    QTimer::singleShot(2000, this, &NetworkTester::sendTestFile);
}

/**
 * @brief 处理接收到的消息
 * @param message 接收到的消息内容
 */
void NetworkTester::onMessageReceived(const QString& message)
{
    qDebug() << "Received message:" << message;
}

/**
 * @brief 处理接收到的文件
 * @param fileName 接收到的文件路径
 */
void NetworkTester::onFileReceived(const QString& fileName)
{
    qDebug() << "Received file saved to:" << fileName;
}

/**
 * @brief 处理发现的新用户
 * @param userInfo 新用户信息
 */
void NetworkTester::onUserDiscovered(const QString& userInfo)
{
    qDebug() << "Discovered:" << userInfo;
}

    /**
     * @brief 广播存在信息
     */
    void NetworkTester::broadcastPresence()
    {
        manager->broadcastPresence(QHostAddress("239.255.43.21"));
        qDebug() << "Multicasting presence...";
        // 每5秒组播一次
        QTimer::singleShot(5000, this, &NetworkTester::broadcastPresence);
    }

/**
 * @brief 发送测试消息
 */
void NetworkTester::sendTestMessage()
{
    manager->sendMessage("Hello from client!");
    qDebug() << "Sent test message";
    
    // 2秒后发送测试文件
    QTimer::singleShot(2000, this, &NetworkTester::sendTestFile);
}

/**
 * @brief 发送测试文件
 */
void NetworkTester::sendTestFile()
{
    // 创建测试文件
    QString testFile = QCoreApplication::applicationDirPath() + "/test.txt";
    QFile file(testFile);
    if(file.open(QIODevice::WriteOnly)) {
        file.write("This is a test file content");
        file.close();
        
        // 发送文件
        manager->sendFile(testFile);
        qDebug() << "Sent test file:" << testFile;
    }
}
