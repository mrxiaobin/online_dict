#include "head.h"

int do_register(int sockfd,sqlite3 *pdb,char *_username,char *_password)
{
	MSG msg;
	char *errmsg;
	char **dbresult;
	int nrow,ncolumn;
	char sql[1024] = {0};
	
	sprintf(sql,"select * from user_table where NAME='%s';",_username);
	if(sqlite3_get_table(pdb,sql,&dbresult,&nrow,&ncolumn,&errmsg) != 0)
	{
		fprintf(stderr,"sqlite3 get table error : %s.\n",errmsg);
		exit(EXIT_FAILURE);
	}
	
	//没有这样的用户名
	if(nrow == 0)
	{
		//录入数据库
		bzero(sql,sizeof(sql));
		sprintf(sql,"insert into user_table values('%s','%s');",_username,_password);
		EXEC_SQL(pdb,sql,errmsg);
		
		msg.type    = REGISTER;
		msg._errno  = OK;
		strcpy(msg.mtext,"Register success !");
		send(sockfd,&msg,sizeof(msg),0);

	//注册失败,用户名存在
	}else{

		msg.type = REGISTER;
		msg._errno = EXIST;
		strcpy(msg.mtext,"Register fail,The username is EEXIST.");
		send(sockfd,&msg,sizeof(msg),0);
	}

	sqlite3_free_table(dbresult);
	
	return 0;
}

int do_login(int sockfd,sqlite3 *pdb,char *_username,char *_password)
{
	MSG msg;
	char *errmsg;
	char **dbresult;
	int nrow,ncolumn;
	char sql[1024] = {0};
	
	sprintf(sql,"select * from user_table where NAME='%s' and PASSWORD='%s';",_username,_password);
	if(sqlite3_get_table(pdb,sql,&dbresult,&nrow,&ncolumn,&errmsg) != 0)
	{
		fprintf(stderr,"sqlite3 get table error : %s.\n",errmsg);
		exit(EXIT_FAILURE);
	}
	
	//没有这样的用户名
	if(nrow == 0)
	{	
		msg.type    = LOGIN;
		msg._errno  = ERROR;
		strcpy(msg.mtext,"The username or password error!");
		send(sockfd,&msg,sizeof(msg),0);

	//登录成功
	}else{
		
		msg.type = LOGIN;
		msg._errno = OK;
		strcpy(msg.mtext,"Login success.");
		send(sockfd,&msg,sizeof(msg),0);
	}

	sqlite3_free_table(dbresult);
	
	return 0;
}

int do_word(int sockfd,sqlite3 *pdb,char * pword)
{
	MSG msg;
	char *errmsg;
	char **dbresult;
	int nrow,ncolumn;
	char sql[1024] = {0};
	
	sprintf(sql,"select * from word_table where WORD='%s';",pword);
	if(sqlite3_get_table(pdb,sql,&dbresult,&nrow,&ncolumn,&errmsg) != 0)
	{
		fprintf(stderr,"sqlite3 get table error : %s.\n",errmsg);
		exit(EXIT_FAILURE);
	}
	
	//没有这样的单词
	if(nrow == 0)
	{	
		msg.type    = WORD;
		msg._errno  = ERROR;
		strcpy(msg.mtext,"No such word ");
		send(sockfd,&msg,sizeof(msg),0);

	//单词存在
	}else{

		msg.type = WORD;
		msg._errno = OK;
		strcpy(msg.mtext,dbresult[ncolumn + 1]);
		send(sockfd,&msg,sizeof(msg),0);
	}

	sqlite3_free_table(dbresult);
	
	return 0;
}

int do_client(int sockfd,sqlite3 *pdb)
{
	MSG msg;
	char *p_username,*p_password;

	while(1)
	{
		recv(sockfd,&msg,sizeof(msg),0);

		switch(msg.type)
		{
		case REGISTER:
			p_password = strdup(msg.password);
			p_username = strdup(msg.username);
			
			do_register(sockfd,pdb,p_username,p_password);	
			
			free(p_username);
			free(p_password);
			break;
		
		case LOGIN:
			p_password = strdup(msg.password);
			p_username = strdup(msg.username);
			
			do_login(sockfd,pdb,p_username,p_password);

			free(p_username);
			free(p_password);
			break;

		case WORD:
			do_word(sockfd,pdb,msg.mtext);
			break;

		defalut:
			exit(EXIT_SUCCESS);
		}	
	}

	return 0;
}
