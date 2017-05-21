#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include <sys/wait.h>

#include "make_log.h"
#include "redis_op.h"

#define ROP_MODULE "test"
#define ROP_PROC "redis_op"


#define MAX_FILE_NUMBER 1000

int main(int argc, char *argv[])
{
    int retn = 0;
    char file_id[128] = {0};


	//读redis

	//创建一个链接句柄
	redisContext *conn = NULL;
	
	const char *lkey = "FILE_ID_LIST";
	const char *h_name_key = "FILEID_FILENAME_HASH";
	const char *h_ctime_key = "FILEID_CTEATETIME_HASH";
	const char *h_owner_key = "FILEID_OWNER_HASH";
	const char *h_dnum_key = "FILEID_DNUM_HASH";
	RVALUES values = NULL;
	int get_num = 0;
	
	conn = rop_connectdb_nopwd("127.0.0.1", "6379");
	if (conn == NULL) {
	    LOG(ROP_MODULE, ROP_PROC, "conn db error");
	    retn = -1;
	    goto END;
	}
	
	
//int rop_range_list(redisContext *conn, char *key, int from_pos, int end_pos, RVALUES values, int *get_num)
	values = malloc(VALUES_ID_SIZE*MAX_FILE_NUMBER*sizeof(char));
	memset(values,0x00,VALUES_ID_SIZE*MAX_FILE_NUMBER*sizeof(char));
	retn = rop_range_list(conn, lkey, 0, -1, values, &get_num);
	if (retn != 0) {
	    LOG(ROP_MODULE, ROP_PROC, "lrange %s error", lkey);
	    goto END;
	}
	
	int i = 0;
	char message[1024];
	char filename[128];
	char createtime[128];
	char owner[128];
	char downloadnumber[128];

	for(i = 0;i<get_num;i++)
	{
		memset(message,0x00,sizeof(message));
		memset(filename,0x00,sizeof(filename));
		memset(createtime,0x00,sizeof(createtime));
		memset(owner,0x00,sizeof(owner));
		memset(downloadnumber,0x00,sizeof(downloadnumber));
		
		strcpy(file_id,values[i]);
		//len = strlen(file_id);
   	 	//file_id[len-1]  = '\0';
   		printf("fileid=[%s]\n", file_id);
	
			
		//int rop_get_hash(redisContext *conn, const char *key, const char* field, char *value);

		retn = rop_get_hash(conn, h_name_key, file_id, filename);
		if (retn != 0) {
		    LOG(ROP_MODULE, ROP_PROC, "get %s error", filename);
		    goto END;
		}
		
		retn = rop_get_hash(conn, h_ctime_key, file_id, createtime);
		if (retn != 0) {
		    LOG(ROP_MODULE, ROP_PROC, "get %s error", createtime);
		    goto END;
		}
		
		retn = rop_get_hash(conn, h_owner_key, file_id, owner);
		if (retn != 0) {
		    LOG(ROP_MODULE, ROP_PROC, "get %s error", owner);
		    goto END;
		}
		
		retn = rop_get_hash(conn, h_dnum_key, file_id, downloadnumber);
		if (retn != 0) {
		    LOG(ROP_MODULE, ROP_PROC, "get %s error", downloadnumber);
		    goto END;
		}
		
		
		sprintf(message,"filename:%s createtime:%s owner:%s,downloadnumber:%s",filename,createtime,owner,downloadnumber);
		LOG(REDIS_LOG_MODULE, REDIS_LOG_PROC, "[+][GMS_REDIS]FILE_INFO | %s |\n", message);
	
	}
	
	
	
	if (conn != NULL) {
	    rop_disconnect(conn);
	}
	
END:
	//垃圾回收
	free(values);
	values = NULL;
	return retn;

}
