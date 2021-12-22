#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERV_IP "220.149.128.100"
#define SERV_PORT 4600
#define BACKLOG 10

#define INIT_MSG "=============================\n Hello! I'm P2P File Sharing Server....\n Please, LOG-IN! \n ============================="
#define LogIn "***** Log-In Success! *****\n"
#define LogFail "***** Failed to Log-In :( *****\n"
#define USER1_ID "user1"
#define USER1_PW "passwd1"
#define USER2_ID "user2"
#define USER2_PW "passwd2"
#define MAX_LEN 100



int main(void)
{
	int sockfd, new_fd;

	struct sockaddr_in my_addr;
	struct sockaddr_in their_addr;
	unsigned int sin_size;

	pid_t pid;

	int rcv_byte;
	char buf[512];
	char List1[512];
	char List2[512];
	char preList1[512] = "";
	char preList2[512] = "";

	char id[200];
	char pw[200];
	char answer[20];
	char newlist[1024];
	char temp[10] = { 0 };

	int val = 1;

	FILE *fp, *result = fopen("FileList.txt","w");

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1){
		perror("Server-socket() error lol!");
		exit(1);
    }
	else printf("Server-socket() sockfd is OK.. \n");

	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(SERV_PORT);
	my_addr.sin_addr.s_addr = INADDR_ANY;

	memset(&(my_addr.sin_zero), 0 , 8);
	
	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&val, sizeof(val)) < 0){
		perror("setsockopt");
		close(sockfd);
		return -1;
	}

    if(bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr))== -1){ 
		perror("Server-bind() error lol!");
 	 	exit(1);											
	}
	else printf("Server-bind() is OK .. \n");


	 if(listen(sockfd, BACKLOG) == -1){
	 	perror("listen() error lol!");								    
	 	exit(1);
	 }	
	 else printf("listen() is OK...\n\n");

while(1)
{
	 sin_size = sizeof(struct sockaddr_in);
	 new_fd = accept(sockfd,(struct sockaddr *)&their_addr, &sin_size);
	
	 pid = vfork();

	 if(pid == 0)
	 {
		printf("Accept() is OK.. \n\n");
		close(sockfd);
		send(new_fd,INIT_MSG,strlen(INIT_MSG) + 1, 0);
	 
		rcv_byte = recv(new_fd, id, sizeof(id), 0);
		rcv_byte = recv(new_fd, pw, sizeof(pw), 0);

		printf("User Information\n");
		printf("ID : %s, PW : %s \n\n",id,pw);

		if(!strcmp(id,USER1_ID) && !strcmp(pw,USER1_PW))
		{
			send(new_fd,LogIn,strlen(LogIn) + 1,0);
			printf(LogIn);
	
			//receive file list from client1
			rcv_byte = recv(new_fd, List1, sizeof(List1), 0);

			//write & update file list on FileList.txt
			printf("user1 File List : \n%s", List1);
			fp=fopen("FileList_user1.txt", "w+");
			fprintf(fp, List1);
			printf("FileList_user1.txt is Updated!\n");
			if(strcmp(preList1, List1))
			{
				result = fopen("FileList.txt","w");
				fprintf(result,preList2);
				strcpy(newlist, List1);
				strcpy(preList1, List1);
				fclose(result);
				printf("FileList.txt is Updated!\n");
			}
			else 
			{
				printf("Unmodified\n");
				strcpy(newlist,"");
			}	
				
		}
		else
		{
			send(new_fd, LogFail,strlen(LogFail) + 1,0);
			printf(LogFail);
			strcpy(id,"");
			strcpy(pw,"");
			exit(1);
		}

		if(!strcmp(id,USER2_ID) && !strcmp(pw,USER2_PW))
		{
				send(new_fd,LogIn,strlen(LogIn) + 1,0);
				printf(LogIn);

				//receive file list from client1
				rcv_byte = recv(new_fd, List2, sizeof(List2), 0);

				//write & update file list on FileList.txt
				printf("user2 File List : \n%s", List2);
				fp=fopen("FileList_user2.txt", "w+");
				fprintf(fp, List2);
				printf("FileList_user2.txt is Updated!\n");
				if(strcmp(preList2, List2))
				{
					result = fopen("FileList.txt","w");
					fprintf(result,preList1);
					strcpy(newlist, List2);
					strcpy(preList2, List2);
					fclose(result);
					printf("FileList.txt is Updated!\n");
				}
				else 
				{
					printf("Unmodified\n");
					strcpy(newlist,"");
				}	
			
		}else
			{
				send(new_fd, LogFail,strlen(LogFail) + 1,0);
				printf(LogFail);
				strcpy(id,"");
				strcpy(pw,"");
				exit(1);
			}
		
		if(newlist != NULL) {
			result = fopen("FileList.txt","a+");
			fprintf(result,newlist);
			fclose(result);

			rcv_byte = recv(new_fd, answer, sizeof(answer), 0);
			if(!strcmp(answer,"yes"))
			{
				int num = 1;
				char sendlist[1024] = "";
				printf("Send FileList.txt\n");
				result = fopen("FileList.txt","r");
				while (1) {
        			char str[MAX_LEN];
        			fgets(str, MAX_LEN, result);
					if(feof(result) == 1) break;
					printf("%s", str);
					sprintf(temp, "%d. ", num++);
					strcat(sendlist,temp);
        			strcat(sendlist,str);
   				}
				send(new_fd, sendlist, strlen(sendlist) + 1, 0);

				fclose(result);
			}
		}
		fclose(fp);
		close(new_fd);
		exit(1);
	 }
	 if(pid > 0)
	 {
		 close(new_fd);
	 }
}
	 close(sockfd);
	 return 0;
}
