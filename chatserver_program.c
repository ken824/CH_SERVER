
/*
Ken Nakazawa

Chat server program in C using queue to save the information of clients and socket communication.
This program icludes function of private chat,test, rename of the client nickname(initially, 
it is a simple integer such as 1,2, or 3), and the list of active client.
How to compile in current OS X:

Server side:
1.Initiate terminal.

2.Go to the directory where you save this program.
command i.e) cd Desktop/chat/chat_server_program.c

3. Compile this program. Type
"gcc -Wall -Werror chatserver_program.c -O2 -lpthread -o chat" and hit enter.
****change term "chatserver_program.c" to the name you saved.

4.Type "./chat" and hit enter to initiate a chat server.

Client side:
1. if you already have telnet in your system, type in terminal,
"telnet <IP address of chat server>  2525" and hit enter. 2525 is the port number I initially set 
in the code, you can modify easily if you want.

**** if you do not have an telnet, go "https://www.wikihow.com/Use-Telnet-on-Mac-OS-X"
   and follow instruction.

***** if you do not know the IP address of chat server, go to the terminal where you will test
      the sever program. Type "ipconfig getifaddr en0" before start chat server program. You should
      see the address of that terminal, which is the IP address of chat server.

2. Enjoy.

*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#define PORT_NUM 2525
#define MAX_NUM	60


static int ID_NUM = 1;
static unsigned int CLIENT_COUNT = 0;


typedef struct {
	struct sockaddr_in addr;	
	int descriptor;			
	char name[20];		
	int ID_NUM;	
} Client_STR;

Client_STR *clients[MAX_NUM];

void Send_MESSAGE(char *s, int ID_NUM){
	for(int i=0;i<MAX_NUM;i++){
		if(clients[i]){
			if(clients[i]->ID_NUM != ID_NUM){
				write(clients[i]->descriptor, s, strlen(s));
			}
		}
	}
}

void Message_to_ALL(char *s){
	for(int i=0;i<MAX_NUM;i++){
		if(clients[i]){
			write(clients[i]->descriptor, s, strlen(s));
		}
	}
}


void Message_Show_self(const char *s, int descriptor){
	write(descriptor, s, strlen(s));
}


void Message_Sending_to_Client(char *s, int ID_NUM){
	for(int i=0;i<MAX_NUM;i++){
		if(clients[i]){
			if(clients[i]->ID_NUM == ID_NUM){
				write(clients[i]->descriptor, s, strlen(s));
			}
		}
	}
}

void List_clients(int descriptor){
	char s[64];
	for(int i=0;i<MAX_NUM;i++){
		if(clients[i]){
			sprintf(s, "[Server Message]<=TOTAL %d | %s\r\n", clients[i]->ID_NUM, clients[i]->name);
			Message_Show_self(s, descriptor);
		}
	}
}

void QUEUE_Adding(Client_STR *cl){
	for(int i=0;i<MAX_NUM;i++){
		if(!clients[i]){
			clients[i] = cl;
			return;
		}
	}
}

void QUEUE_Deleting(int ID_NUM){
	for(int i=0;i<MAX_NUM;i++){
		if(clients[i]){
			if(clients[i]->ID_NUM == ID_NUM){
				clients[i] = NULL;
				return;
			}
		}
	}
}


void Make_Line(char *s){
	while(*s != '\0'){
		if(*s == '\r' || *s == '\n'){
			*s = '\0';
		}
		s++;
	}
}

void Address_Printing(struct sockaddr_in addr){
	printf("%d.%d.%d.%d",
		addr.sin_addr.s_addr & 0xFF,
		(addr.sin_addr.s_addr & 0xFF00)>>8,
		(addr.sin_addr.s_addr & 0xFF0000)>>16,
		(addr.sin_addr.s_addr & 0xFF000000)>>24);
}

void *Client_movement(void *arg){
	char buff_out[1024];
	char buff_in[1024];
	int rlen;

	CLIENT_COUNT++;
	Client_STR *cli = (Client_STR *)arg;

	printf("[Server Message]<= 1 client accepted. client address is ");
	Address_Printing(cli->addr);
	printf(" Client ID is %d\n", cli->ID_NUM);

	sprintf(buff_out, "[Server Message]<= Hello, %s. Welcome to chat server. name yourself by typing '\\RENAME'. To see a help, type '\\HELP'\r\n", cli->name);
	Message_to_ALL(buff_out);

	while((rlen = read(cli->descriptor, buff_in, sizeof(buff_in)-1)) > 0){
	        buff_in[rlen] = '\0';
	        buff_out[0] = '\0';
		Make_Line(buff_in);

		if(!strlen(buff_in)){
			continue;
		}
	
		if(buff_in[0] == '\\'){
			char *command, *param;
			command = strtok(buff_in," ");
			if(!strcmp(command, "\\QUIT")){
				break;
			}else if(!strcmp(command, "\\TEST")){
				Message_Show_self("[Server Message]<=Test success\r\n", cli->descriptor);
			}else if(!strcmp(command, "\\RENAME")){
				param = strtok(NULL, " ");
				if(param){
					char *old_name = strdup(cli->name);
					strcpy(cli->name, param);
					sprintf(buff_out, "[Server Message]<=%s renamed to %s. \r\n", old_name, cli->name);
					free(old_name);
					Message_to_ALL(buff_out);
				}else{
					Message_Show_self("[Server Message]<=invalid input.\r\n", cli->descriptor);
				}
			}else if(!strcmp(command, "\\PRIVATE")){
				param = strtok(NULL, " ");
				if(param){
					int ID_NUM = atoi(param);
					param = strtok(NULL, " ");
					if(param){
						sprintf(buff_out, "[Private Message][%s]", cli->name);
						while(param != NULL){
							strcat(buff_out, " ");
							strcat(buff_out, param);
							param = strtok(NULL, " ");
						}
						strcat(buff_out, "\r\n");
						Message_Sending_to_Client(buff_out, ID_NUM);
					}else{
						Message_Show_self("[Server Message]<=invalid input.\r\n", cli->descriptor);
					}
				}else{
					Message_Show_self("[Server Message]<=invalid input.\r\n", cli->descriptor);
				}
			}else if(!strcmp(command, "\\ACTIVE")){
				sprintf(buff_out, "[Server Message]<=CLIENTS %d\r\n", CLIENT_COUNT);
				Message_Show_self(buff_out, cli->descriptor);
				List_clients(cli->descriptor);
			}else if(!strcmp(command, "\\HELP")){
				strcat(buff_out, "\\HELP     Show help\r\n");
				strcat(buff_out, "\\TEST     Server test\r\n");
				strcat(buff_out, "\\RENAME     <name> Change nickname\r\n");
				strcat(buff_out, "\\PRIVATE  <reference> <message> Send private message\r\n");
				strcat(buff_out, "\\ACTIVE   Show active clients\r\n");
				strcat(buff_out, "\\QUIT     Quit chatroom\r\n");
				Message_Show_self(buff_out, cli->descriptor);
			}else{
				Message_Show_self("[Server Message]<=invalid command.\r\n", cli->descriptor);
			}
		}else{
			sprintf(buff_out, "[%s] %s\r\n", cli->name, buff_in);
			Send_MESSAGE(buff_out, cli->ID_NUM);
		}
	}


	close(cli->descriptor);
	sprintf(buff_out, "[Server Message]<=%s has left chat room. BYE.\r\n", cli->name);
	Message_to_ALL(buff_out);
	QUEUE_Deleting(cli->ID_NUM);
	printf("[Server Message]<= CLient has left chat room, address was");
	Address_Printing(cli->addr);
	printf(" client ID was %d\n", cli->ID_NUM);
	free(cli);
	CLIENT_COUNT--;
	pthread_detach(pthread_self());
	
	return NULL;
}

int main(int argc, char *argv[]){
	int listenfd = 0, descriptor = 0;
	struct sockaddr_in serv_addr;
	struct sockaddr_in cli_addr;
	pthread_t tid;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(PORT_NUM); 

	signal(SIGPIPE, SIG_IGN);
	
	if(bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
		perror("Failed to bind sockets.");
		return 1;
	}

	if(listen(listenfd, 10) < 0){
		perror("Failed to listen sockets.");
		return 1;
	}
    printf("Welcome to chat server program.\n");
	printf("----- SERVER IS NOW READY -----\n");

	while(1){
		socklen_t clilen = sizeof(cli_addr);
		descriptor = accept(listenfd, (struct sockaddr*)&cli_addr, &clilen);

		if((CLIENT_COUNT+1) == MAX_NUM){
			printf("[Server Message]<= Room is full.\n");
			printf("[Server Message]<=rejected.");
			Address_Printing(cli_addr);
			printf("\n");
			close(descriptor);
			continue;
		}

		Client_STR *cli = (Client_STR *)malloc(sizeof(Client_STR));
		cli->addr = cli_addr;
		cli->descriptor = descriptor;
		cli->ID_NUM = ID_NUM++;
		sprintf(cli->name, "%d", cli->ID_NUM);

		QUEUE_Adding(cli);
		pthread_create(&tid, NULL, &Client_movement, (void*)cli);
		sleep(1);
	}
}