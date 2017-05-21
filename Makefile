
CC=gcc
CPPFLAGS= -I./include -I/usr/include/fastdfs -I/usr/include/fastcommon -I/usr/local/include/hiredis
CFLAGS=-Wall 
LIBS= -lfdfsclient -lfastcommon -lhiredis

main=./main

target=$(main) 
read_redis=./read_redis
#test_hiredis=./test/hiredis_test
#test_redisop=./test/redis_op_test


ALL:$(target) $(read_redis)
#$(test_hiredis) $(test_redisop)


#生成所有的.o文件
%.o:%.c
	$(CC) -c $< -o $@ $(CPPFLAGS) $(CFLAGS) 


#main程序
$(main):./main.o ./fdfs_api.o make_log.o redis_op.o
	$(CC) $^ -o $@ $(LIBS)

#read_redis程序
$(read_redis):./read_redis.o make_log.o redis_op.o
	$(CC) $^ -o $@ $(LIBS)


#test_hiredis程序
#$(test_hiredis):./test/hiredis_test.o
#	$(CC) $^ -o $@ $(LIBS)
	
#test_redisop程序
#$(test_redisop):./test/redis_op_test.o make_log.o redis_op.o
#	$(CC) $^ -o $@ $(LIBS)


#clean指令

clean:
	-rm -rf ./*.o $(target) $(read_redis)
#$(test_hiredis) $(test_redisop) ./test/*.o

distclean:
	-rm -rf ./*.o $(target) $(read_redis)
#./test/*.o

#将clean目标 改成一个虚拟符号
.PHONY: clean ALL distclean
