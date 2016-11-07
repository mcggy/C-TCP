#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>

#define BUFFER_SIZE 9000



int main(int argc,char *argv[]){
         printf("main\n");
        
        //printf("%d\n", argc);

        char buffer[BUFFER_SIZE];
        memset(buffer , 0, BUFFER_SIZE);  //每个字节都用0填充
        //printf("%s\n",argv[0]);
        FILE *stream = fopen(argv[0],"r");
       // perror("666");
       // printf("%s\n", getcwd(buffer,BUFFER_SIZE));
        if (stream==NULL)
        {
              printf("User %s is not existing!!\n",argv[0]);
              printf("将退出 ,！ %s", strerror(errno));
              exit(1);
        }
        else
        {
             while(fread(buffer,sizeof(char),BUFFER_SIZE,stream)>0)
            {
                printf("%s",buffer);
                //向客户端发送数据
                if(send(atoi(argv[1]), buffer, strlen(buffer),0)<0)
                {
                    printf("%s Send Failed\n", argv[1]);
                }
                memset(buffer , 0, BUFFER_SIZE);
            }
            fclose(stream);
        }
     
    return 0;
}
