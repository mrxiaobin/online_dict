#include "head.h"

void help_info()
{
	printf("*****************************************\n");
	printf("1.REGISTER\t2.LOGIN\t3.QUIT\n");
	printf("*****************************************\n");

	return;
}

//int do_register(int sockfd,sqlite3 *pdb,char *username,char *password);
int init_tcp(char *ip,char *port)
{
	int sockfd;
	struct sockaddr_in server_addr;
	
	if((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
	{
		perror("Fail to socket");
		exit(EXIT_FAILURE);
	}

	bzero(&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port   = htons(atoi(port));
	server_addr.sin_addr.s_addr = inet_addr(ip);

	if(connect(sockfd,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0)
	{
		perror("Fail to bind");
		exit(EXIT_FAILURE);
	}
	
	return sockfd;
}

int do_register(int sockfd)
{
	MSG msg;

	msg.type = REGISTER;

	printf("Input username : ");
	scanf("%s",msg.username);
	//去掉'\n'
	while(getchar() != '\n');
	
	printf("Input password : ");
	scanf("%s",msg.password);
	//去掉'\n'
	while(getchar() != '\n');
		
	//发给服务器端
	send(sockfd,&msg,sizeof(msg),0);
	usleep(200);

	bzero(&msg,sizeof(msg));
	
	recv(sockfd,&msg,sizeof(msg),0);

	switch(msg._errno)
	{
	case OK:
		printf("%s\n",msg.mtext);
		break;

	case EXIST:
		printf("%s\n",msg.mtext);
		return -1;
	}

	return 0;
}

int do_login(int sockfd)
{
	MSG msg;

	msg.type = LOGIN;

	printf("Input username : ");
	scanf("%s",msg.username);
	//去掉'\n'
	while(getchar() != '\n');
	
	printf("Input password : ");
	scanf("%s",msg.password);
	//去掉'\n'
	while(getchar() != '\n');
		
	//发给服务器端
	send(sockfd,&msg,sizeof(msg),0);
	usleep(200);

	bzero(&msg,sizeof(msg));	
	recv(sockfd,&msg,sizeof(msg),0);

	switch(msg._errno)
	{
	case OK:
		printf("%s\n",msg.mtext);
		break;

	case ERROR:
		printf("%s\n",msg.mtext);
		return -1;
	}

	return 0;
}

int do_word(int sockfd)
{
	MSG msg;
	
	while(1)
	{
		printf("word>");
		msg.type = WORD;
		fgets(msg.mtext,sizeof(msg.mtext),stdin);
		msg.mtext[strlen(msg.mtext) - 1] = '\0';

		send(sockfd,&msg,sizeof(msg),0);
		usleep(200);
		bzero(&msg,sizeof(msg));
		recv(sockfd,&msg,sizeof(msg),0);
		printf("%s\n",msg.mtext);
	}
	
	return 0;
}

int do_task(int sockfd)
{
	int cmd;

	while(1)
	{
		help_info();
		
		printf("cmd>");
		scanf("%d",&cmd);
		
		switch(cmd)
		{
			case 1:
				if(do_register(sockfd) < 0) continue;
				break;

			case 2:
				if(do_login(sockfd) < 0)  continue;
				goto next;
				break;

			case 3:
				exit(EXIT_SUCCESS);

			default:
				printf("Unknow cmd.\n");
				continue;
		}

	}

next:
	do_word(sockfd);
	
	return 0;
}

//./server ip port db
//数据库中已经手动创建了2个表:user_tale,word_table
int main(int argc,char *argv[])
{
	int sockfd;
	int addr_len = sizeof(struct sockaddr);
	struct sockaddr_in peer_addr;

	if(argc < 3)
	{
		fprintf(stderr,"Usage : %s argv[1].\n",argv[0]);
		exit(EXIT_FAILURE);
	}

	sockfd = init_tcp(argv[1],argv[2]);

	do_task(sockfd);
	
	exit(EXIT_SUCCESS);
}
