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
int sock = 0;

void test_fiber() {
    SYLAR_LOG_INFO(g_logger) << "test_fiber";

    sock = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(sock, F_SETFL, O_NONBLOCK);

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr.s_addr);

    if (!connect(sock, (const sockaddr*)&addr, sizeof(addr))) {
    } else if(errno == EINPROGRESS) {
        SYLAR_LOG_INFO(g_logger) << "add event errno=" << errno << " " << strerror(errno);

        sylar::IOManager::GetThis()->addEvent(sock, sylar::IOManager::READ, [](){
            SYLAR_LOG_INFO(g_logger) << "read callback";
            char temp[1000];
            int rt = read(sock, temp, 1000);
            if (rt >= 0) {
                std::string ans(temp, rt);
                SYLAR_LOG_INFO(g_logger) << "read:["<< ans << "]";
            } else {
                SYLAR_LOG_INFO(g_logger) << "read rt = " << rt;
            }
            });
        sylar::IOManager::GetThis()->addEvent(sock, sylar::IOManager::WRITE, [](){
            SYLAR_LOG_INFO(g_logger) << "write callback";
            int rt = write(sock, "GET / HTTP/1.1\r\ncontent-length: 0\r\n\r\n",38);
            SYLAR_LOG_INFO(g_logger) << "write rt = " << rt;
            });
    } else {
        SYLAR_LOG_INFO(g_logger) << "else " << errno << " " << strerror(errno);
    }
}

void test01() {
    sylar::IOManager iom(2, true, "IOM");
    iom.schedule(test_fiber);
}

int main(int argc, char** argv) {
    g_logger->setLevel(sylar::LogLevel::INFO);
    test01();
    
    return 0;
}

