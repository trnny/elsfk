#include<stdio.h>
#include<string.h>
#include<iostream>
#include<mysql/mysql.h>
 
int main()
{
	//初始化MYSQL结构体
	MYSQL* conn=mysql_init(NULL);
	if(NULL==conn)
	{
		printf("errno:%d error:%s\n",mysql_errno(conn),mysql_error(conn));
		return -1;
	}
	//连接MYSQL数据库
	        
if(NULL==mysql_real_connect(conn,"127.0.0.1","root","0414tu..","ELSFK",3306,NULL,0))
	{
		printf("errno:%d error:%s\n",mysql_errno(conn),mysql_error(conn));
		return -1;
	}
	char cmd[256]={};
	while(1)
	{
		printf("mysql>");
		fgets(cmd, 255, stdin);
		cmd[strlen(cmd)] = 0;
		if(0==strcmp("exit",cmd)) break;
		//查询
		if(mysql_query(conn,cmd))
		{
			printf("errno:%d error:%s\n",mysql_errno(conn),mysql_error(conn));
			continue;
		}
		int col=mysql_field_count(conn);
		if(0>col)
		{
			printf("errno:%d error:%s\n",mysql_errno(conn),mysql_error(conn));
			continue;
		}
		//获取结果
		
		MYSQL_RES* res=mysql_store_result(conn);
 
		if(NULL==res)
		{
			printf("errno:%d error:%s\n",mysql_errno(conn),mysql_error(conn));
			continue;
		}
		//从结果中获取一行数据
		MYSQL_ROW row ;
		while(row= mysql_fetch_row(res))
		{
			//显示每一列数据
			for(int i=0;i<col;i++)
			{
				printf("%s ",row[i]);
			}
			printf("\n-------------------------\n");
		}
		mysql_free_result(res);
	}
	mysql_close(conn);
}


// g++ test.cc -o test -lmysqlclient