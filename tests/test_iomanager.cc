/**
 * @file test_iomanager.cc 
 * @brief IO协程调度器测试
 * @details 通过IO协程调度器实现一个简单的TCP客户端，这个客户端会不停地判断是否可读，并把读到的消息打印出来
 *          当服务器关闭连接时客户端也退出
 * @version 0.1
 * @date 2021-06-16
 */
/**
 * @file test_iomanager.cc 
 * @brief IO协程调度器测试
 */
#include "sylar/sylar.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

int sockfd;
void watch_io_read();

// 读事件回调
void do_io_read() {
    SYLAR_LOG_INFO(g_logger) << "read callback";
    char buff[1024] = {0};
    int readlen = read(sockfd, buff, sizeof(buff));
    
    if (readlen > 0) {
        buff[readlen] = '\0';
        SYLAR_LOG_INFO(g_logger) << "read " << readlen << " bytes: " << buff;
        // 重新调度读事件监听
        sylar::IOManager::GetThis()->schedule(watch_io_read);
    } else if (readlen == 0) {
        SYLAR_LOG_INFO(g_logger) << "peer closed connection";
        close(sockfd);
    } else {
        if(errno != EAGAIN && errno != EWOULDBLOCK) {
            SYLAR_LOG_ERROR(g_logger) << "read error: " << strerror(errno);
            close(sockfd);
        }
    }
}

// 写事件回调，用于判断非阻塞connect结果
void do_io_write() {
    SYLAR_LOG_INFO(g_logger) << "write callback";
    int error = 0;
    socklen_t len = sizeof(error);
    
    // 获取套接字错误状态
    if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0) {
        SYLAR_LOG_ERROR(g_logger) << "getsockopt error: " << strerror(errno);
        close(sockfd);
        return;
    }

    if(error) {
        SYLAR_LOG_ERROR(g_logger) << "connect error: " << strerror(error);
        close(sockfd);
        return;
    }
    
    SYLAR_LOG_INFO(g_logger) << "connect success";
    
    // 连接成功后才注册读事件
    sylar::IOManager::GetThis()->addEvent(sockfd, sylar::IOManager::READ, do_io_read);
}



void watch_io_read() {
    SYLAR_LOG_INFO(g_logger) << "watch_io_read";
    sylar::IOManager::GetThis()->addEvent(sockfd, sylar::IOManager::READ, do_io_read);
}

void test_io() {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    SYLAR_ASSERT(sockfd > 0);
    
    // 设置为非阻塞模式
    fcntl(sockfd, F_SETFL, O_NONBLOCK);

    sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(1234);
    inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr.s_addr);

    int rt = connect(sockfd, (const sockaddr*)&servaddr, sizeof(servaddr));
    
    // 修正点1：正确处理connect返回值
    if(rt == 0) {
        // 极少数情况：立即连接成功
        SYLAR_LOG_INFO(g_logger) << "connect immediately";
        sylar::IOManager::GetThis()->addEvent(sockfd, sylar::IOManager::READ, do_io_read);
    } else if(rt == -1) {
        if(errno == EINPROGRESS) {
            // 正常情况：连接正在进行中
            SYLAR_LOG_INFO(g_logger) << "EINPROGRESS";
            
            // 修正点2：只注册写事件监听连接结果
            sylar::IOManager::GetThis()->addEvent(
                sockfd, 
                sylar::IOManager::WRITE, 
                do_io_write
            );
        } else {
            // 真实错误
            SYLAR_LOG_ERROR(g_logger) << "connect error: " << strerror(errno);
            close(sockfd);
        }
    }
}

void test_iomanager() {
    sylar::IOManager iom(2);  // 使用2个线程的IO管理器
    iom.schedule(test_io);
}

int main(int argc, char** argv) {
    sylar::EnvMgr::GetInstance()->init(argc, argv);
    sylar::Config::LoadFromConfDir(sylar::EnvMgr::GetInstance()->getConfigPath());
    test_iomanager();
    return 0;
}