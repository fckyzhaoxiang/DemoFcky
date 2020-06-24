#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>

#define PORT 8888
#define MAXDATASIZE 100

int main()
{
 	int sockfd,num;
	struct sockaddr_in server;
	socklen_t addrlen;
	
	if((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1)
	{
		perror("socket() err\n");
		exit(1);
	}
	
	//int opt = SO_REUSEADDR;
	//setsocketopt(listenfd,SOL_SOCKET,SO_REUSEADD,&opt,sizof(opt));
	//bzero(&server, sizeof(server));

	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	


	while(1)
	{
		if(connect(sockfd,(struct sockaddr*)&server, sizeof(struct sockaddr_in)) == -1)
		{
			//perror("accept() err\n");
			//exit(1);
			continue;
		}
		while(1){
			char sendBuf[100] = {0};
			
#if 1
			printf("send:");
			fgets(sendBuf,sizeof(sendBuf),stdin);
			//sendBuf[strlen(sendBuf) - 1] = '\0';
			write(sockfd,sendBuf,sizeof(sendBuf));
#endif
#if 0
			memset(sendBuf, '\0', sizeof(sendBuf));
			int ret = read(sockfd,sendBuf,sizeof(sendBuf));
			//if(ret > 0)
				printf("recv:%s\n,ret=%d\n",sendBuf,ret);
#endif

		}
	}
}
