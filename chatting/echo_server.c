#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<pthread.h>
#include<signal.h>

#define BUF_SIZE 1024
void *clnt_connection(void *arg);
void send_message(char *message, int len);
void error_handling(char *message);
void sig_handler(int signo);

int clnt_number = 0;
void *thread_rtn;
pthread_mutex_t mutx;
struct sigaction act;


struct clnt_info 
{
	char address[20];
	char name[20];
	int clnt_socks;
	pthread_t thread;
};

struct clnt_info users[10];

int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	int clnt_adr_sz, i;
	

	if(argc!=2) {
		printf("Usage : %s <prot>\n", argv[0]);
		exit(1);
	}

	if(pthread_mutex_init(&mutx, NULL))
		error_handling("pthread_mutex_init() error");

	serv_sock=socket(PF_INET, SOCK_STREAM, 0);
	if(serv_sock==-1)
		error_handling("socket() error");

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_adr.sin_port=htons((unsigned short)atoi(argv[1]));

	if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
		error_handling("bind() error");

	if(listen(serv_sock, 10)==-1)
		error_handling("listen() error");

	//signal(SIGHUP,(void *)sig_handler); // case - hangup
        //signal(SIGINT,(void *)sig_handler); // case - ^C input
        //signal(SIGQUIT,(void *)sig_handler); // case - ^\ input
        //signal(SIGTERM,(void *)sig_handler); // case - killsystme call
        //signal(SIGSTOP,(void *)sig_handler); // case - ^Z input
	memset(&act, 0x00, sizeof(struct sigaction));
	act.sa_handler = sig_handler;
	sigaction(SIGINT, &act, NULL);

	while(1){	
	clnt_adr_sz=sizeof(clnt_adr);
	clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
	
	pthread_mutex_lock(&mutx);
	clnt_number++;
	pthread_mutex_unlock(&mutx);

	for(i=0;i<sizeof(users);i++){
		if(users[i].clnt_socks == 0){
		pthread_create(&users[i].thread, NULL, clnt_connection, (void *)(intptr_t)clnt_sock);
	        read(clnt_sock, users[i].name, sizeof(struct clnt_info));
		strcpy(users[i].address, inet_ntoa(clnt_adr.sin_addr));
	        users[i].clnt_socks=clnt_sock;
		break;
		}
	}
	printf("New client connected, IP: %s User : %s \n", users[i].address, users[i].name);
	printf("Total User : %d \n",clnt_number);
	}
	return 0;
}

void *clnt_connection(void *arg)
{
	int clnt_sock = (int)(intptr_t)arg;
	int str_len =0, i;
	char message[BUF_SIZE];
	
	while((str_len=read(clnt_sock, message, sizeof(message))) > 0)
		send_message(message, str_len);

	for(i=0 ; i<sizeof(users); i++){

		if(clnt_sock == users[i].clnt_socks){
		        printf("IP : %s => User : %s Out \n",users[i].address, users[i].name);
			memset(users[i].address, 0, sizeof(users[i].address));
			memset(users[i].name, 0, sizeof(users[i].name));
			users[i].clnt_socks = 0;
			break;
		}
	}
	pthread_mutex_lock(&mutx);
	clnt_number--;
        pthread_mutex_unlock(&mutx);

	printf("Total User : %d \n",clnt_number);	

	shutdown(clnt_sock, SHUT_RDWR);
	close(clnt_sock);
	return 0;
}

void send_message(char *message, int len){

	int i;

	pthread_mutex_lock(&mutx);
	for(i=0; i<clnt_number+1; i++)
	write(users[i].clnt_socks, message, len);
	pthread_mutex_unlock(&mutx);
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

void sig_handler(int signo)
{
	int i;
	char message[BUF_SIZE]="Server close() \n";
	send_message(message, sizeof(message));
	for(i=0; i<(sizeof(users)/sizeof(users[0]));i++){
		if(users[i].clnt_socks == 0)
		;
		else {
		shutdown(users[i].clnt_socks, SHUT_RDWR);
		pthread_join(users[i].thread, NULL);
		}
	}
	printf("All Client Close\n");
	kill(getpid(),signo);
	pthread_mutex_destroy(&mutx);
	exit(1);
}
