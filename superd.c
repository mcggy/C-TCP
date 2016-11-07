#include <stdio.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/wait.h>
#include <errno.h>

#define PORT 8000//端口号
#define RECV_SIZE 64//接收客户端输入数据的最大长度
int main(int argc , char *argv[])
{
    pid_t pid1,pid2;//子进程
    int dup_fd;//用于重定位
    char argsockfd[20];//用来传递的文件标识符参数
    int client_sock;//accept后的新的套接口
    struct sockaddr_in client_addr;
    socklen_t client_addr_size = sizeof(client_addr);
    //select函数用到的变量
    fd_set rfds;//标识符集合
    struct timeval tv;//设置时间
    int maxfd,retval,len;
    //用于接收用户数据
    char receive[RECV_SIZE];
    //服务器套接口
    int serv_sock = socket(AF_INET,SOCK_STREAM,0);
    if(serv_sock<0){
        printf("Create Socket Failed!!\n");
        exit(1);
    }
    //将套接字和IP、端口绑定
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));  //每个字节都用0填充
    serv_addr.sin_family = AF_INET;  //使用IPv4地址
    serv_addr.sin_addr.s_addr = htons(INADDR_ANY);  
    serv_addr.sin_port = htons(PORT);  //端口
    if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)))
    {
        printf("Server Bind Port : %d Failed!", PORT); 
        exit(1);
    }
     //进入监听状态，等待用户发起请求
    if(listen(serv_sock, 20))
    {
        printf("Server Listen Failed!"); 
        exit(1);
    }
    //循环接受用户请求
    while(1)
    {
        //接收用户连接请求
        if((client_sock = accept(serv_sock, (struct sockaddr*)&client_addr, &client_addr_size))<0)
        {
        	printf("accept error\n");
        	continue;
        }
         //用子进程处理接收的请求
        if((pid1 = fork())==0)
        {
	//子进程不需要监听,关闭后服务器主进程依然在监听
        	close(serv_sock);
        	while (1) 
	{
            		/* 把集合清空 */
            		FD_ZERO(&rfds);
            		/* 把当前连接句柄client_sock加入到集合中 */
            		FD_SET(client_sock, &rfds);
            		if (client_sock > maxfd)
                	maxfd = client_sock;
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
            		} else 
            		{
		if (FD_ISSET(client_sock, &rfds)) 
		{
                    	/* 当前连接的socket上有消息到来则接收对方发过来的消息并显示 */
                    	bzero(receive, RECV_SIZE);
                    	/* 接收客户端的消息 */
                    	len = recv(client_sock, receive, RECV_SIZE, 0);
                    	if (len > 0)
                    	{
			//打印确认收到数据无误
                    		printf("%s\n",receive+1 );
			//将int型的描述符转换为char
			snprintf(argsockfd, sizeof(argsockfd), "%d", client_sock);
			//传给execvp的参数
			char *arguments[] = {receive+1,argsockfd,0};
			//fork子进程执行可执行文件
			if((pid2=fork())==0)
			{
				if (receive[0]=='0')       		
				{       			
					if(execvp("/home/mcg/Code/Final1/Getaccount",arguments)<0)
						printf("Execvp Error\n");
				}        			
				if (receive[0]=='1')
				{
					if(execvp("/home/mcg/Code/Final1/FetchPage",arguments)<0)
						printf("Execvp Error\n");
				}
				exit(0);
			}
		}else //接收失败退出
		{
                        		break;
                    	}
                    	}
                	}
              }
        	close(client_sock);
        	exit(0);
        }else if (pid1>0)//父进程
        {
        	close(client_sock);
        }
        
        
            
    }
    close(serv_sock);
    return 0;
	
	
}