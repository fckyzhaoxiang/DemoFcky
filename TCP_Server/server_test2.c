#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>

#define PORT 8888
#define BACKLOG 1
#define MAX_CLIENT_NUM 30

int main()
{
    int listenfd,connectfd;
	struct sockaddr_in server;
	struct sockaddr_in client;
	socklen_t addrlen;
	
	int client_fd[MAX_CLIENT_NUM];
	int max_fd = -1;
	int ret;
	struct timeval timeout;
	

	if((listenfd = socket(AF_INET,SOCK_STREAM,0)) == -1)
	{
		perror("socket() err\n");
		exit(1);
	}
	
	//int opt = SO_REUSEADDR;
	//setsocketopt(listenfd,SOL_SOCKET,SO_REUSEADD,&opt,sizof(opt));
	//bzero(&server, sizeof(server));

	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if(bind(listenfd, (struct sockaddr*)&server,sizeof(struct sockaddr_in)) == -1)
	{
		perror("bind() err\n");
		exit(1);
	}

	if(listen(listenfd, BACKLOG) < 0)
	{
		perror("listen() err\n");
		exit(1);
	}
	
	fd_set read_set;
	fd_set write_set;
	fd_set select_read_set;

	FD_ZERO(&read_set);
	FD_ZERO(&write_set);
	FD_ZERO(&select_read_set);
	
	for(int i=0; i < MAX_CLIENT_NUM; i++)
	{
		client_fd[i] = -1;
	}
	
	max_fd = listenfd;
	FD_SET(listenfd, &read_set);
	
	while(1){
		
		timeout.tv_sec = 60;
		timeout.tv_usec = 0;
		
		max_fd = listenfd;
		for(int i= 0; i < MAX_CLIENT_NUM; i++)
		{
			if(max_fd < client_fd[i])
				max_fd = client_fd[i];
		}
		
		select_read_set = read_set;
		ret = select(max_fd+1,&select_read_set, NULL, NULL, &timeout);
		if(ret == 0)
		{
			printf("timeout\n");
		}else if(ret < 0)
		{
			printf("error occur\n");
		}else
		{
			if(FD_ISSET(listenfd, &select_read_set))
			{
				printf("new client comes\n");
				addrlen = sizeof(struct sockaddr);
				connectfd = accept(listenfd,(struct sockaddr*)&client, &addrlen);
				if(connectfd < 0)		
				{
					perror("accept() err\n");
					exit(1);
				}else
				{
					for(int i=0; i < MAX_CLIENT_NUM; i++)
					{
						if(client_fd[i] == -1)
						{
							client_fd[i] = connectfd;
							FD_SET(connectfd, &read_set);
							break;
						}
						if(max_fd < connectfd)
							max_fd = connectfd;
					}
				}
			}else
			{
				for(int i = 0; i < MAX_CLIENT_NUM; i++)
				{
					if(client_fd[i] == -1)
						continue;
					char buf[256] = {0};
					if(FD_ISSET(client_fd[i], &select_read_set))
					{
#if 1
						int num = read(client_fd[i],buf,256);
						if(num > 0)
							printf("read:%s\n",buf);
#endif
						printf("send:");
						fgets(buf,sizeof(buf),stdin);
						//scanf("%s",sendBuf);
						buf[strlen(buf) - 1] = '\0';
						write(client_fd[i],buf,sizeof(buf));
						printf("send:%s\n",buf);
					}
				}
			}	
		}
		
	}
	
	close(listenfd);
	close(connectfd);	

}
