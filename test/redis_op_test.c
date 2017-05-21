#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "make_log.h"
#include "redis_op.h"

#define ROP_MODULE "test"
#define ROP_PROC "redis_op"

int main(int argc, char *argv[])
{
    int retn = 0;
    //创建一个链接句柄
    redisContext *conn = NULL;

    const char *key = "my_list";
    const char * field = "wakaka";
    char value[1024];

    conn = rop_connectdb_nopwd("127.0.0.1", "6379");
    if (conn == NULL) {
        LOG(ROP_MODULE, ROP_PROC, "conn db error");
        retn = -1;
        goto END;
    }

    retn = rop_list_push(conn, key, "XXX");
    if (retn != 0) {
        LOG(ROP_MODULE, ROP_PROC, "lpush %s error", key);
        goto END;
    }

	key = "kongbawa";
//int rop_set_string(redisContext *conn, const char *key, char *value);
	retn = rop_set_string(conn, key, "YYY");
    if (retn != 0) {
        LOG(ROP_MODULE, ROP_PROC, "set %s error", key);
        goto END;
    }
    
//int rop_get_string(redisContext *conn, const char *key, char *value);
	memset(value,0x00,sizeof(value));
	retn = rop_get_string(conn, key, value);
    if (retn != 0) {
        LOG(ROP_MODULE, ROP_PROC, "get %s error", key);
        goto END;
    }
    printf("value = %s\n",value);
    
    
    key = "kongbawa2";
//int rop_set_hash(redisContext *conn, const char *key, const char* field, char *value);
	retn = rop_set_hash(conn, key, field, "ZZZZZZZZZ");
    if (retn != 0) {
        LOG(ROP_MODULE, ROP_PROC, "set %s error", key);
        goto END;
    }
    
//int rop_get_hash(redisContext *conn, const char *key, const char* field, char *value);
	memset(value,0x00,sizeof(value));
	retn = rop_get_hash(conn, key, field, value);
    if (retn != 0) {
        LOG(ROP_MODULE, ROP_PROC, "get %s error", key);
        goto END;
    }
    printf("value = %s\n",value);
    
    

    if (conn != NULL) {
        rop_disconnect(conn);
    }

END:
    //垃圾回收

	return retn;
}