#ifndef _HEAD_H_
#define _HEAD_H_ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sqlite3.h>
#include <signal.h>


//消息的类型
#define REGISTER  100 
#define LOGIN     200 
#define WORD      300 
#define QUIT      400 

//错误码的类型
#define OK		  10 
#define EXIST     20 
#define ERROR     30

struct user_info
{
	char name[15];
	char password[20];
};

typedef struct
{
	//REGISTER,LOGIN,WORD,QUIT
	int type;
	
	//OK(成功/正确),EXIST(用户名已经存在),ERROR(用户名和密码不正确)
	int _errno;
	
	union 
	{
		struct user_info u_info;
		char buf[1024];
	}content;
//客户端填单词，服务器端填单词解释
#define mtext 		content.buf 
#define username	content.u_info.name 
#define password  	content.u_info.password

}MSG;

#define EXEC_SQL(db,sql,errmsg) do{\
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) < 0)\
	{\
		fprintf(stderr,"sqlite3 execl [%s] error : %s.\n",sql,errmsg);\
		exit(EXIT_FAILURE);\
	}\
}while(0);

#endif
