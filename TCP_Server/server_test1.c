#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/fcntl.h>

#define PORT 8888
#define BACKLOG 1

int main()
{
    int listenfd,connectfd;
	struct sockaddr_in server;
	struct sockaddr_in client;
	socklen_t addrlen;

	int backlog = 3;
	int maxfd;
	int new_cli_fd;
	int watch_fd_list[backlog+1];
	int ci;
	
	for(ci = 0; ci <= backlog; ci++)
		watch_fd_list[ci] = -1;
	
	if((listenfd = socket(AF_INET,SOCK_STREAM,0)) == -1)
	{
		perror("socket() err\n");
		exit(1);
	}
	
	//int opt = SO_REUSEADDR;
	//setsocketopt(listenfd,SOL_SOCKET,SO_REUSEADD,&opt,sizof(opt));
	//bzero(&server, sizeof(server));

	if(fcntl(listenfd, F_SETFL, O_NONBLOCK) == -1)
	{
		perror("fcntl() fail\n");
		close(listenfd);
		exit(1);
	}
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


	watch_fd_list[0] = listenfd;
	maxfd = listenfd;

	//
	fd_set watchset;
	FD_ZERO(&watchset);
	FD_SET(listenfd, &watchset);

	struct timeval tv;
	
	while(1){
		tv.tv_sec = 20;
		tv.tv_usec = 0;

		FD_ZERO(&watchset);
		FD_SET(listenfd, &watchset);
		
		for(ci = 0; ci <= backlog; ci++)
		{
			if(watch_fd_list[ci] != -1)
			{
				FD_SET(watch_fd_list[ci], &watchset);
			}
		}		

		int rcd = select(maxfd + 1, &watchset,NULL,NULL, &tv);
		switch(rcd)
		{
			case -1:
				printf("Select error\n");
				exit(1);
				break;
			case 0:
				printf("Select time_out\n");
				FD_ZERO(&watchset);
				for(ci = 1; ci <= backlog; ci++)
				{
					shutdown(watch_fd_list[ci],2);
					watch_fd_list[ci] = -1;
				}
				FD_CLR(listenfd, &watchset);
				FD_SET(listenfd, &watchset);
				continue;
			default:
				if(FD_ISSET(listenfd, &watchset))
				{
					addrlen = 0;
					connectfd = accept(listenfd,(struct sockaddr*)&client, &addrlen);
					if(connectfd < 0)		
					{
						perror("accept() err\n");
						exit(1);
					}
					printf("\nopen communication with Client %s on socket %d\n", inet_ntoa(client.sin_addr), connectfd);
					
					for(ci = 1; ci<= backlog; ci++)
					{
						if(watch_fd_list[ci] == -1)
						{
							watch_fd_list[ci] = connectfd;
							break;
						}
					}
					
					FD_SET(connectfd, &watchset);
					if(maxfd < connectfd)
					{
						maxfd = connectfd;
					}
					continue;
				}else
				{
					for(ci = 1; ci <= backlog; ci++)
					{
						if(watch_fd_list[ci] == -1)
							continue;
						if(!FD_ISSET(watch_fd_list[ci], &watchset))
							continue;					
					}
					
					char buf[128] = "fdsfdsfdsfsd\n";
					//printf("send:%s\n",buf);
					//fgets(buf,sizeof(buf),stdin);
					//scanf("%s",sendBuf);
					buf[strlen(buf) - 1] = '\0';
					write(watch_fd_list[ci],buf,sizeof(buf));
					printf("send:%s\n",buf);

				}

		}

	}
	
	close(listenfd);
	close(connectfd);	

}
