# 网络模块说明文档

## 概述
本模块实现了一个基于Qt的网络通信系统，支持文本消息传输、文件传输和服务发现功能。使用TCP协议进行可靠数据传输，UDP协议进行服务发现。

## 核心类说明

### NetworkManager
- **构造函数**: NetworkManager(QObject *parent = nullptr)
- **主要功能**:
  - `bool startServer(quint16 basePort)`: 启动服务器
  - `void connectToPeer(const QString &ip, quint16 port)`: 连接到对等节点
  - `void sendMessage(const QString &message)`: 发送文本消息
  - `void sendFile(const QString &filePath)`: 发送文件
  - `void joinMulticastGroup(const QHostAddress &groupAddress)`: 加入组播组
  - `void leaveMulticastGroup(const QHostAddress &groupAddress)`: 离开组播组
  - `void broadcastPresence(const QHostAddress &groupAddress)`: 广播存在信息

- **信号**:
  - `void newMessageReceived(const QString &message)`: 收到新消息
  - `void newFileReceived(const QString &fileName)`: 收到新文件
  - `void newUserDiscovered(const QString &userInfo)`: 发现新用户

### NetworkTester
- **构造函数**: NetworkTester(NetworkManager* manager, QObject *parent = nullptr)
- **主要功能**:
  - `void start()`: 启动测试
  - `void broadcastPresence()`: 广播存在信息
  - `void sendTestMessage()`: 发送测试消息
  - `void sendTestFile()`: 发送测试文件

## 文件传输实现

### 发送端流程
1. 读取文件信息（大小、名称等）
2. 将文件信息序列化并发送
3. 分块读取文件内容并发送
4. 等待接收端确认

### 接收端流程
1. 接收文件信息
2. 创建目标文件
3. 接收文件数据块并写入
4. 发送接收确认

## 组播功能实现

### 组播地址配置
- 默认组播地址：239.255.43.21
- 默认组播端口：45454

### 组播消息处理
1. 创建UDP Socket
2. 加入组播组
3. 接收组播消息
4. 处理组播消息
5. 发送组播消息
6. 离开组播组

## 使用示例

### 启动服务器
```cpp
NetworkManager manager;
if (manager.startServer(12345)) {
    qDebug() << "Server started on port 12345";
}
```

### 连接客户端
```cpp
NetworkManager manager;
manager.connectToPeer("127.0.0.1", 12345);
```

### 发送消息
```cpp
manager.sendMessage("Hello World!");
```

### 发送文件
```cpp
manager.sendFile("/path/to/file.txt");
```

## 测试说明

### 启动测试
```cpp
NetworkManager manager;
NetworkTester tester(&manager);
tester.start();
```

### 测试流程
1. 启动服务器
2. 客户端连接
3. 发送测试消息
4. 发送测试文件
5. 验证接收结果

## 性能优化
- 使用QDataStream进行高效数据序列化
- 采用异步I/O模型提高并发性能
- 实现消息分块传输支持大文件传输
- 使用连接池管理TCP连接
- 优化内存使用，减少内存拷贝

## 注意事项
- 确保端口号未被占用
- 注意文件大小限制
- 测试时确保服务器和客户端在同一网络环境
- 定时器间隔可根据需要调整
- 测试文件默认保存在应用程序目录下

## 已知问题
1. 大文件传输时内存占用较高
2. 跨网段服务发现可能失败
3. 网络断开重连机制待完善

## 未来改进
- 增加传输加密支持
- 实现断点续传功能
- 支持多线程并发传输
- 添加流量控制机制
