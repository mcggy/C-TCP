#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>

#define PORT 8000
#define SIZE 64
#define RECV_SIZE 10000

int main(int argc,char **argv){
    fd_set rfds;
    struct timeval tv;
    int retval, maxfd = -1;
    int len;
    if (argc!=2)
    {
        printf("Usage:%s ServerAddress\n", argv[0]);
        exit(1);
    }
    //客户端套接口
    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(client_addr));  //每个字节都用0填充
    client_addr.sin_family = AF_INET;  //使用IPv4地址
    client_addr.sin_addr.s_addr = htons(INADDR_ANY);
    client_addr.sin_port = htons(0);  //自动端口
    //创建套接字
    int csock = socket(AF_INET, SOCK_STREAM, 0);
    if(csock<0){
        printf("Create Socket Failed!!\n");
        exit(1);
    }
    //绑定
    if (bind(csock,(struct sockaddr*)&client_addr,sizeof(client_addr)))
    {
        printf("Client Bind Port Failed!\n"); 
        exit(1);
    }
    //向服务器（特定的IP和端口）发起请求
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));  //每个字节都用0填充
    serv_addr.sin_family = AF_INET;  //使用IPv4地址
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  //具体的IP地址
    serv_addr.sin_port = htons(PORT);  //端口
    //发送连接请求
    if(connect(csock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))<0)
    {
        printf("Conn Failed\n");
        exit(1);
    }
    //接受用户输入的数据
    char buffer[SIZE];
    char receive_content[RECV_SIZE];

    printf("please input flag(0 for account and 1 for Page )+content\n");
    
    //循环接发数据直至退出
    while(1)
    {

        /* 把集合清空 */
        FD_ZERO(&rfds);
        /* 把标准输入句柄0加入到集合中 */
        FD_SET(0, &rfds);
        maxfd = 0;
        /* 把当前连接句柄sockfd加入到集合中 */
        FD_SET(csock, &rfds);
        if (csock > maxfd)
            maxfd = csock;
        /* 设置最大等待时间 */
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        /* 开始等待 */
        retval = select(maxfd + 1, &rfds, NULL, NULL, &tv);
        if (retval == -1) {
            printf("将退出，select出错！ %s", strerror(errno));
            break;
        } else if (retval == 0) {
            /* printf
               ("没有任何消息到来，用户也没有按键，继续等待……\n"); */
            continue;
        } else {

            if (FD_ISSET(csock, &rfds)) {
                /* 连接的socket上有消息到来则接收对方发过来的消息并显示 */
                bzero(receive_content, RECV_SIZE);
                /* 接收对方发过来的消息，最多接收 MAXBUF 个字节 */
                len = recv(csock, receive_content, RECV_SIZE, 0);
                if (len > 0)
                    printf("%s\n",receive_content);
                // else {
                //     //break;
                // }
            }
            if (FD_ISSET(0, &rfds)) {
                /* 用户按键了，则读取用户输入的内容发送出去 */

                bzero(buffer, SIZE);
                fgets(buffer, SIZE, stdin);
                /* 发消息给服务器 */
                len = send(csock, buffer, strlen(buffer)-1, 0);
                if(len>0)
                     printf("Send Success!!\n");  
            }
        }
    }
        
    //关闭套接字
    close(csock);
    
    return 0;
}

