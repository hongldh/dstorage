#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>


#include "hiredis.h"


int main(int argc,char * argv[])
{
	
	redisContext * conn = NULL;
	
	conn = redisConnect("127.0.0.1",6379);
	if(conn == NULL || conn->err)
	{
		if(conn)
		{
			printf("Error:%s\n",conn->errstr);
		}
		else
		{
			printf("Can't allocate redis context\n");
		}
	}
	
	redisReply * reply = NULL;
	reply = redisCommand(conn,"SET %s %s","test_string","helloHiredis");
	
	if(reply == NULL)
	{
		printf("set error\n");
	}
	
	printf("set %s success!\n","test_string");
	
	freeReplyObject(reply);
	
	reply = redisCommand(conn,"GET %s","test_string");
	if(reply == NULL)
	{
		printf("get error\n");
		goto END;
	}
	
	printf("value = %s,len = %d\n",reply->str,(int)reply->len);
	freeReplyObject(reply);
	
	char * list_key = "my_test_list";
	int list_len = 0;
	
	reply = redisCommand(conn,"lpush %s %s",list_key,"python");
	if(reply == NULL)
	{
		printf("lpush error\n");
		goto END;
	}
	list_len = reply->integer;
	
	printf("lpush success len = %d\n",list_len);
	
	freeReplyObject(reply);
	
	int i = 0;
	reply = redisCommand(conn,"lrange %s 0 -1",list_key);
	if(reply == NULL)
	{
		printf("lrange error\n");
		goto END;
	}
	
	for(i = 0;i<reply->elements;i++)
	{
		printf("[%d]:%s\n",i,reply->element[i]->str);
	}
	freeReplyObject(reply);
	
	
END:
	if(conn != NULL)
	{
		redisFree(conn);
	}		
	
	return 0;
}