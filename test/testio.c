#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/time.h>
#include <errno.h>
#include <signal.h>
#include <stddef.h>
#include <sys/un.h>

#define MEM_SPACE 20000000
#define NET_SPACE 10000000
#define EACH_WRITE 10000

#define DISK_SPACE 50000000
#define EACH_DISK_WRITE  1000000

static char sbuf[MEM_SPACE];
static int g_port = 10305;

void test_pipe()
{
	pid_t pid;
	char *sbuf, *tbuf;
	struct timeval tm_start, tm_end;
	int cfd[2];   //child write, parent read
	int pfd[2];   //parent write
	int i, cnt;
	char *p;
	int ret, total=0;
	sbuf = (char*)malloc(MEM_SPACE);
    tbuf = (char*)malloc(MEM_SPACE + 1000000);

	pipe(pfd);
	pipe(cfd);
	for(i=0; i<100000; i++) {
		sbuf[i] = rand()%127;
	}

	if ((pid=fork()) < 0) {
		printf("fork error\n");
		exit(0);
	} else if (pid == 0) {
		close(cfd[0]);	
		close(pfd[1]);
		cnt = MEM_SPACE / EACH_WRITE;
		do {
			ret = read(pfd[0], tbuf+total, EACH_WRITE);
			if (ret > 0) {
				//for(i=0; i<ret/4000; i++) {
				//	*(tbuf+total+i*4000) = 3;
				//}
				total += ret;
			}
		} while (ret > 0 && total <= MEM_SPACE-1);
		write(cfd[1], "done", 5);
		free(sbuf);
		free(tbuf);
		exit(0);
	} else {
		//parent process
		long timeuse;
		cnt = MEM_SPACE / EACH_WRITE;   //1M every write
		close(cfd[1]);
		close(pfd[0]);
		sleep(1);
		gettimeofday(&tm_start, NULL);
		for (i=0; i<cnt; i++) {
			write(pfd[1], sbuf, EACH_WRITE);
		}
		read(cfd[0], tbuf, 100);
		gettimeofday(&tm_end, NULL);
		timeuse = 10000 * (tm_end.tv_sec - tm_start.tv_sec) + (tm_end.tv_usec - tm_start.tv_usec) / 100;

		printf("test_pipe. time: %d.%d ms\n", timeuse/10, timeuse%10);
		wait(NULL);
	}
	free(sbuf);
	free(tbuf);
}

//test domain socket
void test_domain()
{
	pid_t pid;
	int sockfd;
	char *sbuf, *tbuf;
	socklen_t clilen;
	int listenfd, connfd;
	struct sockaddr_un cliaddr, servaddr;

	struct timeval tm_start, tm_end;
	const char *bind_dsock = "./ff.sock";

	int cfd[2];   //child write, parent read
	int i, cnt;
	char *p;
	int ret, total=0;
	int dsock_len;
	
	//init socket
	
	sbuf = (char*)malloc(MEM_SPACE);
    tbuf = (char*)malloc(MEM_SPACE + 1000000);

	pipe(cfd);
	for(i=0; i<100000; i++) {
		sbuf[i] = rand()%127;
	}

	if ((pid=fork()) < 0) {
		printf("fork error\n");
		exit(0);
	} else if (pid == 0) {
		int timeuse;
		//child process;
		//connect to parent
		//unlink(bind_dsock);
		connfd = socket(AF_LOCAL, SOCK_STREAM, 0);
		bzero(&cliaddr, sizeof(cliaddr));
		cliaddr.sun_family = AF_LOCAL;
		clilen = sizeof(cliaddr);
		strcpy(cliaddr.sun_path, bind_dsock);
		bind(connfd, (struct sockaddr*)&cliaddr, clilen);
		close(cfd[0]);	
		cnt = MEM_SPACE / EACH_WRITE;
		gettimeofday(&tm_start, NULL);


		if (connect(connfd, (struct sockaddr*)&cliaddr, clilen) < 0) {
			printf("connect failed\n");
			exit(1);
		}

		gettimeofday(&tm_end, NULL);

		timeuse = 10000 * (tm_end.tv_sec - tm_start.tv_sec) + (tm_end.tv_usec - tm_start.tv_usec) / 100;
		printf("connect waste. time: %d.%d ms\n", timeuse/10, timeuse%10);
		do {
			ret = read(connfd, tbuf+total, EACH_WRITE);

			if (ret > 0) {
				//for(i=0; i<ret/4000; i++) {
				//	*(tbuf+total+i*4000) = 3;
				//}

				total += ret;
			}
		} while (ret > 0 && total <= MEM_SPACE-1);
		write(cfd[1], "done", 5);
		free(sbuf);
		free(tbuf);
		exit(0);
	} else {
		//parent process
		long timeuse;

		unlink(bind_dsock);
		listenfd = socket(AF_LOCAL, SOCK_STREAM, 0);
		bzero(&servaddr, sizeof(servaddr));
		servaddr.sun_family = AF_LOCAL;
		strcpy(servaddr.sun_path, bind_dsock);
		bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr));

		cnt = MEM_SPACE / EACH_WRITE;   //1M every write
		close(cfd[1]);
		
		listen(listenfd, 5);
		connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &clilen);
		if (connfd < 0) {
			printf("accept error\n");
			exit(1);
		}

		gettimeofday(&tm_start, NULL);
		for (i=0; i<cnt; i++) {
			write(connfd, sbuf, EACH_WRITE);
		}
		read(cfd[0], tbuf, 100);
		close(connfd);
		gettimeofday(&tm_end, NULL);
		timeuse = 10000 * (tm_end.tv_sec - tm_start.tv_sec) + (tm_end.tv_usec - tm_start.tv_usec) / 100;

		printf("test_domain_socket. time: %d.%d ms\n", timeuse/10, timeuse%10);
		wait(NULL);
	}
	free(sbuf);
	free(tbuf);

}

//test local tcp socket
void test_localtcp()
{
	pid_t pid;
	int sockfd;
	char *sbuf, *tbuf;
	socklen_t clilen;
	int listenfd, connfd;
	struct sockaddr_in cliaddr, servaddr;

	struct timeval tm_start, tm_end;
	const char *bind_dsock = "127.0.0.1";

	int cfd[2];   //child write, parent read
	int i, cnt;
	char *p;
	int ret, total=0;
	int dsock_len;
	int svrport = 10315;	
	int optval = 1;
	//init socket
	
	sbuf = (char*)malloc(MEM_SPACE);
    tbuf = (char*)malloc(MEM_SPACE + 1000000);

	pipe(cfd);
	for(i=0; i<100000; i++) {
		sbuf[i] = rand()%127;
	}

	if ((pid=fork()) < 0) {
		printf("fork error\n");
		exit(0);
	} else if (pid == 0) {
		//child process;
		//connect to parent
		connfd = socket(AF_INET, SOCK_STREAM, 0);
		bzero(&cliaddr, sizeof(cliaddr));
		cliaddr.sin_family = AF_INET;
		cliaddr.sin_port = htons(g_port);
		inet_pton(AF_INET, "127.0.0.1", &cliaddr.sin_addr);
		clilen = sizeof(cliaddr);
		sleep(2);

		close(cfd[0]);	
		cnt = MEM_SPACE / EACH_WRITE;
		//connect localtcp
		if (connect(connfd, (struct sockaddr*)&cliaddr, sizeof(cliaddr)) < 0) {
			printf("connect tcp failed\n");
			exit(1);
		}
		do {
			ret = read(connfd, tbuf+total, EACH_WRITE);
			if (ret > 0) {
				//for(i=0; i<ret/4000; i++) {
				//	*(tbuf+total+i*4000) = 3;
				//}

				total += ret;
			}
		} while (ret > 0 && total <= MEM_SPACE-1);
		write(cfd[1], "done", 5);
		free(sbuf);
		free(tbuf);
		exit(0);
	} else {
		//parent process
		long timeuse;
		cnt = MEM_SPACE / EACH_WRITE;   //1M every write
		close(cfd[1]);

		listenfd = socket(AF_INET, SOCK_STREAM, 0);
		bzero(&servaddr, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
		servaddr.sin_port = htons(g_port);
		if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,(char *) &optval, sizeof(optval)) == -1)
		{
			exit(1);
		}
		bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
		listen(listenfd, 5);

		connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &clilen);
		if (connfd < 0) {
			printf("accept error\n");
			exit(1);
		}

		gettimeofday(&tm_start, NULL);
		for (i=0; i<cnt; i++) {
			write(connfd, sbuf, EACH_WRITE);
		}
		read(cfd[0], tbuf, 100);
		close(connfd);
		gettimeofday(&tm_end, NULL);
		timeuse = 10000 * (tm_end.tv_sec - tm_start.tv_sec) + (tm_end.tv_usec - tm_start.tv_usec) / 100;

		printf("test_socket. time: %d.%d ms\n", timeuse/10, timeuse%10);
		wait(NULL);
	}
	free(sbuf);
	free(tbuf);

}


void test_memcpy()
{
   char *sbuf;   
   struct timeval tm_start, tm_end;
   char *tbuf;
   long timeuse;
   int i;

   	sbuf = (char*)malloc(MEM_SPACE);
    tbuf = (char*)malloc(MEM_SPACE);
    if (tbuf == NULL || sbuf == NULL) {
		printf("no more memory\n");
	}
	for(i=0; i<100000; i++) {
		sbuf[i] = rand()%127;
	}
    gettimeofday(&tm_start, NULL);
	memcpy(tbuf, sbuf, MEM_SPACE-1);
	gettimeofday(&tm_end, NULL);
    timeuse = 10000 * (tm_end.tv_sec - tm_start.tv_sec) + (tm_end.tv_usec - tm_start.tv_usec) / 100;
	printf("test_memcpy. time: %d.%d ms\n", timeuse/10, timeuse%10);
	free(sbuf);
	free(tbuf);
}

void test_seq_disk(int flag)
{
	int i;
	char *sbuf;
	FILE *fp;
	int fd;
	struct timeval tm_start, tm_end;
	long timeuse;
	int cnt;

	sbuf = (char*)malloc(MEM_SPACE);

	for(i=0; i<100000; i++) {
		sbuf[i] = rand()%127;
	}

	fp = fopen("dd_seq", "w+");
	if (fp == NULL) return;
	fd = fileno(fp);
	
	gettimeofday(&tm_start, NULL);
	for(i=0; i<DISK_SPACE/EACH_DISK_WRITE; i++) {
		write(fd, sbuf, EACH_DISK_WRITE);
	}
	if (flag == 0) fsync(fd);
	gettimeofday(&tm_end, NULL);
    timeuse = 10000 * (tm_end.tv_sec - tm_start.tv_sec) + (tm_end.tv_usec - tm_start.tv_usec) / 100;
	printf("test_disk %d. time: %d.%d ms\n", flag, timeuse/10, timeuse%10);
	free(sbuf);

}

void test_random_disk(int flag)
{
	int i, fd;
	char fname[100];
	FILE *fp;
	static char sbuf[10000];
	struct timeval tm_start, tm_end;
	long timeuse;

	memset(sbuf, '3', 10000);
	gettimeofday(&tm_start, NULL);

	for (i=0; i<2000; i++) {
		sprintf(fname, "dd_%d", i);
		fp = fopen(fname, "w+");
		fd = fileno(fp);
		write(fd, sbuf, 10000);
		if (flag == 0) fsync(fd);
		fclose(fp);
	}
	gettimeofday(&tm_end, NULL);
    timeuse = 10000 * (tm_end.tv_sec - tm_start.tv_sec) + (tm_end.tv_usec - tm_start.tv_usec) / 100;
	printf("test_scatter_file %d. time: %d.%d ms\n", flag, timeuse/10, timeuse%10);

	gettimeofday(&tm_start, NULL);

		sprintf(fname, "dd_log");
		fp = fopen(fname, "a+");
		fd = fileno(fp);
		for (i=0; i<2000; i++) {
			write(fd, sbuf, 10000);
			if (flag == 0) fsync(fd);
		}
		fclose(fp);
	gettimeofday(&tm_end, NULL);
    timeuse = 10000 * (tm_end.tv_sec - tm_start.tv_sec) + (tm_end.tv_usec - tm_start.tv_usec) / 100;
	printf("test_log %d. time: %d.%d ms\n", flag, timeuse/10, timeuse%10);


}

//remote client
void test_remote()
{
	int total=0, ret;
	int sockfd;
	char *sbuf, *tbuf;
	socklen_t clilen;
	int listenfd, connfd;
	struct sockaddr_in cliaddr, servaddr;

	tbuf = (char*)malloc(NET_SPACE + 1000000);
	connfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&cliaddr, sizeof(cliaddr));
	cliaddr.sin_family = AF_INET;
	cliaddr.sin_port = htons(g_port);
	inet_pton(AF_INET, "220.181.28.172", &cliaddr.sin_addr);
	//inet_pton(AF_INET, "192.168.11.137", &cliaddr.sin_addr);
	clilen = sizeof(cliaddr);
	if (connect(connfd, (struct sockaddr*)&cliaddr, sizeof(cliaddr)) < 0) {
			printf("connect tcp failed\n");
			exit(1);
	}
	do {
		ret = read(connfd, tbuf+total, EACH_WRITE);
		if (ret > 0) {
			total += ret;
		}
	} while (ret > 0 && total <= NET_SPACE-1);
	write(connfd, "done", 5);
	sleep(2);
}

//监听进程
void start_svr()
{
	socklen_t clilen;
	int listenfd, connfd;
	struct sockaddr_in cliaddr, servaddr;
	int cnt, ret, total=0;
	struct timeval tm_start, tm_end;
	int optval = 1;
	int i, timeuse;
    static char sbuf[NET_SPACE+EACH_WRITE];

	clilen = sizeof(cliaddr);
	cnt = NET_SPACE / EACH_WRITE;   //1M every write
    clilen = sizeof(cliaddr);
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(g_port);
	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,(char *) &optval, sizeof(optval)) == -1)
	{
		exit(1);
	}
	bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
	listen(listenfd, 5);

	connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &clilen);
	if (connfd < 0) {
		perror("accept error");
		exit(1);
	}

	gettimeofday(&tm_start, NULL);
	for (i=0; i<cnt; i++) {
		write(connfd, sbuf, EACH_WRITE);
	}
    printf("buf send over\n");
	ret = read(connfd, sbuf, 100);
	gettimeofday(&tm_end, NULL);

	close(connfd);
	timeuse = 10000 * (tm_end.tv_sec - tm_start.tv_sec) + (tm_end.tv_usec - tm_start.tv_usec) / 100;

	printf("test_remote_net. time: %d.%d ms\n", timeuse/10, timeuse%10);

}

#define RANDOM_SPACE 50000000 
//极端的随机写
void test_random_write()
{
	struct timeval tm_start, tm_end;
	int i, timeuse;
    static char sbuf[RANDOM_SPACE]; //10MB
	int fd;
	FILE *fp;
	int cnt;
	int pos;
	//write 10MB first
	memset(sbuf, '1', RANDOM_SPACE);
	fp = fopen("dd_random", "w+");
	fd = fileno(fp);
	write(fd, sbuf, RANDOM_SPACE);
    fsync(fd);	
	fclose(fp);

	fp = fopen("dd_random", "r+");
   	if (NULL == fp) {
		printf("file dd_random not ready\n");
		exit(1);
	}	
	fd = fileno(fp);
	cnt = 10000;
	gettimeofday(&tm_start, NULL);
	//seek and right
	for (i=0; i<cnt; i++) {
		pos = rand() % (RANDOM_SPACE-1000);
		fseek(fp, pos, SEEK_SET);
		write(fd, sbuf, 1000); //write 1000 bytes
		fsync(fd);
	}
	gettimeofday(&tm_end, NULL);

	fclose(fp);
	timeuse = 10000 * (tm_end.tv_sec - tm_start.tv_sec) + (tm_end.tv_usec - tm_start.tv_usec) / 100;

	printf("test_remote_net. time: %d.%d ms\n", timeuse/10, timeuse%10);

}

int main(int argc, char ** argv)
{
	char ch;
	if (argc == 0) {
		test_domain();
		test_localtcp();
		test_pipe();
		test_memcpy();
	} else {
		ch = getopt(argc, argv, "mcsdanr");
		switch(ch) {
			case 'm':
				test_domain();
				test_localtcp();
				test_pipe();
				test_memcpy();
				break;
			case 'd': 
				test_seq_disk(0);
				test_random_disk(0);
				break;
			case 'n':
				test_seq_disk(1);
				test_random_disk(1);
				break;
			case 'a':
				test_domain();
				test_localtcp();
				test_pipe();
				test_memcpy();
				test_seq_disk(1);
				test_random_disk(1);
				break;
			case 'r':
				test_random_write();
				break;
			case 'c':
				printf("connect remote svr\n");
				test_remote();
				break;
			case 's':
				printf("start listen svr\n");
				start_svr();
				break;
			default:
				printf("should add param: -a all\n-d disk\n-n disk no sync");
				printf("\n-m memory\n");
				printf("-c tcp client\n-s tcp server\n");
				break;
		}
	}
	sleep(1);
	return 0;
}

