#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <sys/stat.h>

#define SERV_IP "220.149.128.100"
#define SERV_PORT 4531
#define CLT1_IP "220.149.128.101"
#define CLT1_PORT 4532
#define CLT2_IP "220.149.128.103"
#define CLT2_PORT 4533

#define LogIn "***** Log-In Success! *****\n"

#define MAX_LEN 100


int main(int argc, char *atgv[])
{
	//=============Declaration of variables.=================
	FILE *fd;
	fd = fopen("ShareList.txt","w+");

	DIR* dp = NULL;
	struct dirent* dir = NULL;
	struct stat DirBuf;

	int sockfd;
	int index = 1;
	struct sockaddr_in dest_addr;
	
	int rcv_byte;

	char buf[512];
	char id[200];
	char pw[200];
	char List[512] = "";
	char file_name[10][10];
	char temp[10] = { 0 };
	char answer[10];
	int num;

	//=================================================

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1)
	{
		perror("Client-socket() error lol!");
		exit(1);
	}
	else printf("Client-socket() sockfd is OK ... \n");

	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(SERV_PORT);
	dest_addr.sin_addr.s_addr = inet_addr(SERV_IP);

	memset(&(dest_addr.sin_zero), 0 , 8);

	if(connect(sockfd,(struct sockaddr *)&dest_addr,sizeof(struct sockaddr)) == -1)
	{
		perror("Client-connect() error lol");
		exit(1);
	}
	else printf("Client-connect() is OK... \n\n");

	// Receive Log-in Success message
	rcv_byte = recv (sockfd, buf, sizeof(buf),0);
	printf("%s\n", buf);

	printf("ID : ");
	scanf("%s",id);
	send(sockfd, id ,strlen(id)+1, 0);

	printf("PW : ");
	scanf("%s",pw);
	send(sockfd, pw, strlen(pw)+1, 0);

	rcv_byte = recv(sockfd, buf, sizeof(buf), 0);
	printf("%s\n", buf);
	
	// After Log-in Successe, Send File List to Server
	if(!strcmp(buf,LogIn))
	{
		if((dp = opendir("/home/st2016146025/FinalProject/Share")) == NULL){		// Directory Open Error
			printf("directroy open error\n");
			return -1;
		}

		printf("Here's lists you shared!\n");
		while((dir = readdir(dp)) != NULL){		// read File from head to end
			if(!strcmp(".",dir->d_name)||!strcmp("..",dir->d_name))continue;	// . , .. Directory Exception
			printf("%d : %s \n",index,dir->d_name);		// print Share Directory's Files name

			strcat(List, dir->d_name);
			strcat(List,"\t");

			strcat(List, CLT1_IP);
			strcat(List, "\t");

			sprintf(temp, "%d", CLT1_PORT);
			strcat(List, temp);
			strcat(List, "\t");

			strcat(List, "USER1");
			strcat(List, "\n");
			index++;
			/*================================================
			* Make File List							 	 |
			-- Send File list it this Form --				 |
			* ex)							  				 |
				a.txt CLT1_IP CLT1_PORT			USER1		 |
				b.txt CLT1_IP CLT1_PORT			USER1		 |
				c.txt CLT1_IP CLT1_PORT			USER1		 |
			=================================================*/
		}
		//send list on log-in
		rcv_byte = recv(sockfd, buf, sizeof(buf), 0);
		if(!strcmp(buf,"Y")) send(sockfd, List, strlen(List) + 1, 0 );
		else exit(1);
		
		printf("Do you want to get a FileList?(yes/no) : ");
		scanf("%s", &answer);
		if(strcmp(answer,"yes")) exit(1);
		send(sockfd, answer, strlen(answer) + 1, 0);	
		rcv_byte = recv(sockfd, buf, sizeof(buf), 0);
		printf("this is FileList.txt\n%s",buf);
		fprintf(fd,buf);
		fclose(fd);

		printf("What do you want? : ");
		scanf("%d",&num);
		fd = fopen("ShareList.txt","r");
		
		while (1) {
        	char str[MAX_LEN];
        	fgets(str, MAX_LEN, fd);
			if((num + '0') == str[0]) {
				printf("%s\n", str);
				break;
			}
			if(feof(fd) == 1)
			{
				printf("File Not Found");
				break;
			} 

   		}
		fclose(fd);

		closedir(dp);
	}
	close(sockfd);

	return 0;
}
