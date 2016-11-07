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
#include <errno.h>

#define BUFFER_SIZE 10000
#define URL_SIZE 64




int main(int argc,char *argv[]){
        //相当于客户端套接字(因为它访问互联网其他的网站接收数据)
        struct sockaddr_in addr;  
        //解析域名
        struct hostent *pURL;  
        char myurl[URL_SIZE];  
        char *pHost = 0;  
        char host[URL_SIZE], GET[URL_SIZE];  
        //保存用户请求
        char header[BUFSIZ] = ""; 
        //标志字符串 
        char hp[] = "://";
        //截取输入的网址
        char *url = strstr(argv[0],hp)+3;
        //存储接收数据
        char text[BUFFER_SIZE]; 
       
       

        strcpy(myurl, url);  
        //分离主机中的主机地址和相对路径
        for (pHost = myurl; *pHost != '/' && *pHost != '\0'; ++pHost);  
        //获取相对路径保存到GET中
        if ( (int)(pHost - myurl) == strlen(myurl) )  
                strcpy(GET, "/");  
        else  
                strcpy(GET, pHost);  
        //将主机信息保存到host中
        //此处将它置零，即它所指向的内容里面已经分离出了相对路径，剩下的为host信
        //息(从myurl到pHost地址段存放的是HOST)
        *pHost = '\0';  
        strcpy(host, myurl);  
        printf("%s\n%s\n", host, GET);  


        
        int sockweb = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        //将上面获得的主机信息通过域名解析函数获得域名信息  
        pURL = gethostbyname(host);  
        addr.sin_family = AF_INET;  
        addr.sin_addr.s_addr = *((unsigned long*)pURL->h_addr);  
        addr.sin_port = htons(80);  


        strcat(header, "GET ");  
        strcat(header, GET);  
        strcat(header, " HTTP/1.1\r\n");  
        strcat(header, "HOST: ");  
        strcat(header, host);  
        strcat(header, "\r\nConnection: Close\r\n\r\n");  
        printf("发起连接的http请求是：%s",header); 

        printf("开始连接。。。。"); 
        connect(sockweb,(struct sockaddr *)&addr,sizeof(addr));  
         printf("开始发送。。。。"); 
        send(sockweb, header, strlen(header), 0);  
        printf("开始接受。。。。"); 
        memset(text,0,BUFFER_SIZE);
        
        while ( recv(sockweb, text, BUFFER_SIZE, 0) > 0)  
        {  
                // printf("%s", text);  
            printf("接受完毕。。。。"); 
            send(atoi(argv[1]),text,BUFFER_SIZE,0);
            memset(text,0,BUFFER_SIZE);

        } 
        //close(fd); 
        
        
        close(sockweb);  
        return 0;
}