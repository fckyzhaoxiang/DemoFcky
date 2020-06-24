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
	
	int max_fd = -1;
	struct timeval timeout;
Test:
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
	
	fd_set read_set;
	fd_set write_set;

	FD_ZERO(&read_set);
	FD_ZERO(&write_set);

	max_fd = sockfd;
	FD_SET(sockfd, &read_set);
	printf("-----\n");
	while(1)
	{
		if(connect(sockfd,(struct sockaddr*)&server, sizeof(struct sockaddr_in)) == -1)
		{
			sleep(2);
			perror("accept() err\n");
			//exit(1);
			continue;
		}
		printf("++++\n");
		while(1){
			char sendBuf[100] = {0};
			timeout.tv_sec = 20;
			timeout.tv_usec = 0;				
			FD_ZERO(&read_set);
			max_fd = sockfd;
			FD_SET(sockfd, &read_set);
			
			int ret = select(max_fd+1,&read_set,NULL,NULL, &timeout);
			if(ret == 0)
			{
				printf("timetou\n");
				continue;
			}
			else if(ret < 0)
			{
				printf("error select\n");
			}
			else
			{
				if(FD_ISSET(sockfd, &read_set))
				{
					memset(sendBuf, '\0', sizeof(sendBuf));
					int num = read(sockfd,sendBuf,sizeof(sendBuf));
					printf("recv:%s\n,num=%d\n",sendBuf,num);
					if(num <= 0)
					{
						close(sockfd);
						goto Test;
					}
				}
			}		
#if 0
			printf("send:");
			fgets(sendBuf,sizeof(sendBuf),stdin);
			sendBuf[strlen(sendBuf) - 1] = '\0';
			write(sockfd,sendBuf,sizeof(sendBuf));
#endif
#if 0
			memset(sendBuf, '\0', sizeof(sendBuf));
			read(sockfd,sendBuf,sizeof(sendBuf));
			printf("recv:%s\n,ret=%d\n",sendBuf,ret);
#endif

		}
	}
}
