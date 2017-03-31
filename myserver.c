#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/sendfile.h>
#include<fcntl.h>


char webpage[]="HTTP/1.1 200 OK\r\n"
"Content-type:text/html; charset:UTF-8\r\n\r\n"
"<!DOCTYPE html>\r\n";


int main(int argc,char *argv[])
{
	struct sockaddr_in server_addr,client_addr;
	socklen_t sin_len=sizeof(client_addr);
	int fd_server,fd_client;
	char buf[2048];
	char *filename,*res;
	int on=1,fdimg,fdfile;

	fd_server=socket(AF_INET,SOCK_STREAM,0);

	if(fd_server<0)
	{
		perror("socket");
		exit(1);
	}

	setsockopt(fd_server,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(int));
	server_addr.sin_family=AF_INET;
	server_addr.sin_addr.s_addr=INADDR_ANY;
	server_addr.sin_port=htons(8080);

	if(bind(fd_server,(struct sockaddr*)&server_addr,sizeof(server_addr)) == -1)
	{
		perror("bind");
		close(fd_server);
		exit(1);
	}

	if(listen(fd_server,10) == -1)
	{
		perror("listen");
		close(fd_server);
		exit(1);
	} 

	while(1)
	{
		fd_client=accept(fd_server,(struct sockaddr*)&client_addr,&sin_len);
		
		if(fd_client == -1)
		{
			perror("connection failed...\n");
			continue;
		} 
		
		printf("client connected...\n");
		
		if(!fork())
		{
			close(fd_server);
			memset(buf,0,2048);
			read(fd_client,buf,2047);
			printf("%s",buf);

			if(!strncmp(buf,"GET /favicon.ico",16))
			{
				fdimg=open("favicon.ico",O_RDONLY);
				sendfile(fd_client,fdimg,NULL,125000);
                                close(fdimg);
			}

			else if(!strncmp(buf,"GET /abc.jpg",12))
                        {
                                fdimg=open("abc.jpg",O_RDONLY);
                                sendfile(fd_client,fdimg,NULL,240000);
                                close(fdimg);
                        }
			/*else
			{
				strcpy(filename,&buf[5]);
				res = strtok(filename, " ");
				fdfile=open(res,O_RDONLY);
				sendfile(fd_client,fdfile,NULL,200);
				close(fdfile);
				printf("hello");
			}*/
			else{
				send(fd_client,webpage,sizeof(webpage),on);
				fdfile=open("index.html",O_RDONLY);
				sendfile(fd_client,fdfile,NULL,300);
				close(fdfile);			
			}
			
			close(fd_client);
			printf("closing");
			exit(0);
		
		}	//end of if
		
		close(fd_client);
	}	//end of while
	
	return (0);
}
