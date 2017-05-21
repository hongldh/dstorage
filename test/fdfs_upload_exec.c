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


int main(int argc, char *argv[])
{
    int retn = 0;
    int pfd[2];
    char file_id[128] = {0};

    retn = pipe(pfd);
    pid_t pid;

    if (argc < 2) {
        printf("./a.out filename \n");
        exit(1);
    }

    if (retn < 0) {
        perror("pipe error\n");
        exit(1);
    }

    pid = fork();
    if (pid == 0)
    {
        //child
        //关闭一些fd
        close(pfd[0]);
        //dup stdout ->pfd[1]
        dup2(pfd[1], STDOUT_FILENO);
        //exec ---> fdfs_upload_file  /etc/fdfs/client.conf  argv[1]
        execlp("fdfs_upload_file", "fdfs_upload_file", "/etc/fdfs/client.conf", argv[1], NULL);
        perror("exec fdfs_upload_file error");
    }
    else
    {
        int len = 0;
        //parent
        //关闭一些fd
        close(pfd[1]);
        //wait
        wait(NULL);

        //read(pfd[0]) --->buf
        read(pfd[0], file_id, 128);
        len = strlen(file_id);
        file_id[len-1]  = '\0';
        printf("fileid=[%s]\n", file_id);
        close(pfd[0]);

        
        
        
	    //入redis
	   	
	    int retn = 0;
	    //创建一个链接句柄
	    redisContext *conn = NULL;
	
	    const char *lkey = "FILE_ID_LIST";
	    const char *h_name_key = "FILEID_FILENAME_HASH";
	    const char *h_ctime_key = "FILEID_CTEATETIME_HASH";
	    const char *h_owner_key = "FILEID_OWNER_HASH";
	    const char *h_dnum_key = "FILEID_DNUM_HASH";
	
	    conn = rop_connectdb_nopwd("127.0.0.1", "6379");
	    if (conn == NULL) {
	        LOG(ROP_MODULE, ROP_PROC, "conn db error");
	        retn = -1;
	        goto END;
	    }
	
	    retn = rop_list_push(conn, lkey, file_id);
	    if (retn != 0) {
	        LOG(ROP_MODULE, ROP_PROC, "lpush %s error", lkey);
	        goto END;
	    }
	
	
	//int rop_set_hash(redisContext *conn, const char *key, const char* field, char *value);
		retn = rop_set_hash(conn, h_name_key, file_id, argv[1]);
	    if (retn != 0) {
	        LOG(ROP_MODULE, ROP_PROC, "set %s error", h_name_key);
	        goto END;
	    }
	    
		retn = rop_set_hash(conn, h_ctime_key, file_id, "00/00/00_00:00:00");
	    if (retn != 0) {
	        LOG(ROP_MODULE, ROP_PROC, "set %s error", h_ctime_key);
	        goto END;
	    }
	    
	    retn = rop_set_hash(conn, h_owner_key, file_id, "kongbawa");
	    if (retn != 0) {
	        LOG(ROP_MODULE, ROP_PROC, "set %s error", h_owner_key);
	        goto END;
	    }
	    
	    retn = rop_set_hash(conn, h_dnum_key, file_id, "1");
	    if (retn != 0) {
	        LOG(ROP_MODULE, ROP_PROC, "set %s error", h_dnum_key);
	        goto END;
	    }
	    
	    
	
	    if (conn != NULL) {
	        rop_disconnect(conn);
	    }
	
	END:
	    //垃圾回收
	
		return retn;
	        
	}
	
	
	return 0;
}
