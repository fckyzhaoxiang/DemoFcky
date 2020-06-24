#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#define PORT 8888
#define BACKLOG 1

int main()
{
    int listenfd,connectfd;
	struct sockaddr_in server;
	struct sockaddr_in client;
	socklen_t addrlen;
	time_t now;
	struct tm t2;
	int year,month,day,hour,min, sec;
	float Angle_A,Angle_B;


	
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

		pid_t pid = fork();
		if(pid < 0)
			continue;
		else if(pid == 0)
		{
			
			printf("get connect,ip is %s, potr is %d\n", inet_ntoa(client.sin_addr), connectfd);
			while(1){
				char buf[256] = {0};
				now = time(0);
				localtime_r(&now, &t2);
				year = t2.tm_year + 1900;
				month = t2.tm_mon + 1;
				day = t2.tm_mday;
				hour = t2.tm_hour;
				min = t2.tm_min;
				sec = t2.tm_sec;
				Angle_A = 0.001;
				Angle_B = 0.002;
				sleep(5);
				sprintf(buf,"%04d%02d%02d%02d%02d%02d,%f,%f,",year,month,day,hour,min,sec,Angle_A,Angle_B);			
			

				buf[strlen(buf) - 1] = '\0';
				write(connectfd,buf,sizeof(buf));
				printf("time");
				printf("send:%s\n",buf);

				}
			close(listenfd);
			close(connectfd);
		}else
		{
			//close(listenfd);
			close(connectfd);
			
		}
	}
	
	close(listenfd);
	close(connectfd);	

}
