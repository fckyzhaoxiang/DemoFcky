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

int main()
{
    int listenfd,connectfd;
	struct sockaddr_in server;
	struct sockaddr_in client;
	socklen_t addrlen;
	
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



	while(1){
		
		addrlen = 0;
		connectfd = accept(listenfd,(struct sockaddr*)&client, &addrlen);
		if(connectfd < 0)		
		{
			perror("accept() err\n");
			exit(1);
			continue;
		}
		char buf_ip[INET_ADDRSTRLEN];
		memset(buf_ip, '\0', sizeof(buf_ip));
		inet_ntop(AF_INET,&client.sin_addr,buf_ip,sizeof(buf_ip));
		printf("get connect,ip is %s, potr is %d\n", buf_ip, ntohs(client.sin_port));
		while(1){
			char buf[256] = {0};
			memset(buf,'\0',sizeof(buf));
#if 1
			int ret = read(connectfd,buf,sizeof(buf));
			if(ret > 0)
				printf("recv:%s\n",buf);
#endif
#if 0			
			printf("send:");
			fgets(buf,sizeof(buf),stdin);
			//scanf("%s",sendBuf);
			
			buf[strlen(buf) - 1] = '\0';
			write(connectfd,buf,sizeof(buf));
			printf("send:%s\n",buf);
#endif
		}

	}
	
	close(listenfd);
	close(connectfd);	
	
	return 0;

}
