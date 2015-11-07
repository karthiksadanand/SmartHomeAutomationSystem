#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<string.h>

int main(int argc, char *argv[]){

	int sd, bindval,ssd,read_flag,write_flag,n;
	char buff[256],buff1[200];
	struct sockaddr_in client_addr, server_addr;
	socklen_t clilength;
	int backlog=5;
	read_flag=write_flag=0;

	sd=socket(PF_INET,SOCK_STREAM,0);
	if(sd<0){
		printf("socket failure\n");
		exit(1);
	}

	bzero((char *) &server_addr, sizeof(server_addr));

	server_addr.sin_family=AF_INET;
	server_addr.sin_port=htons(atoi(argv[1]));
	server_addr.sin_addr.s_addr=INADDR_ANY;
	bindval=bind(sd,(struct sockaddr *) &server_addr,sizeof(server_addr));

	if(bindval<0){
		printf("Binding failure");
		exit(1);
	}
	listen(sd,backlog);

	clilength=sizeof(client_addr);

	while(1){

		ssd=accept(sd,(struct sockaddr *) &client_addr,&clilength);

		if(ssd<0){
			printf("Error entering in opening a slave socket\n");
			exit(1);
		}

		printf("%s client with port number %d is connected to  server at port number with %s server with port number %d \n",inet_ntoa(client_addr.sin_addr),htons(client_addr.sin_port),inet_ntoa(server_addr.sin_addr),htons(server_addr.sin_port));
		bzero(buff,256);

		while(1){

			read_flag=read(ssd,buff,255);

			if(read_flag<0){
				printf("Error in reading\n");
			}

			printf("The buffer in Server is:: %s\n",buff);

			if(!strcmp(buff, "quit"))
			{
			  printf("Server quitting..\n");
			  int i = 1;
			  send(ssd, &i, sizeof(int), 0);
			  exit(0);
			}

			char *token;
			token=strtok(buff,"|");
			int flag=0,client_id=0, water_val=0,gas_val=0,elec_val=0;
			char time_stamp[200],cid[20];
			bzero(time_stamp,200);

			 while(token!=NULL)
			 {
				 if(0==flag)
				 {
					 water_val=atoi(token);
				 }
				 else if(1==flag)
				 {
					 gas_val= atoi(token);
				 }
				 else if(2==flag)
				 {
					 elec_val= atoi(token);
				 }
				 else if(3==flag)
				 {
					 strcpy(time_stamp,token);
				 }
				 else if(4==flag)
				 {
					 client_id= atoi(token);
					 strcpy(cid,token);

				 }
				 flag++;
				 token=strtok(NULL,"|");
			 }

			FILE *f ;
			char str[20];
			strcpy(str,"User ");
			strcat(str,cid);
			strcat(str,".txt");
			f = fopen(str, "w");
			if (f == NULL)
			 {
			   printf("Error opening file!\n");
			   exit(1);
			 }
			fprintf(f,"water\tgas\telectricity\ttime_day\t\tclient_id\n");
			fprintf(f,"%d\t%d\t%d\t\t%s\t\t%d\n",water_val,gas_val,elec_val,time_stamp,client_id);

			printf("Water usage: %d\n",water_val);
			printf("Gas Usage: %d\n",gas_val);
			printf("Electricity Usage: %d\n",elec_val);
			printf("Time: %s\n",time_stamp);
			printf("ID: %d\n",client_id);

			bzero(buff,sizeof(buff));
			strcpy(buff,"hello");
			write(ssd,buff,strlen(buff));

			fclose(f);

		}

	}

	return 0;
}




/*

#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<stdlib.h>
#include<arpa/inet.h>

int main(int argc, char *argv[]){

	int sd, bindval,ssd,read_flag,write_flag,n;
	char buff[256],buff1[200];
	struct sockaddr_in client_addr, server_addr;
	socklen_t clilength;
	int backlog=5;
	read_flag=write_flag=0;

	sd=socket(PF_INET,SOCK_STREAM,0);
	if(sd<0){
		printf("socket failure\n");
		exit(1);
	}

	bzero((char *) &server_addr, sizeof(server_addr));

	server_addr.sin_family=AF_INET;
	server_addr.sin_port=htons(atoi(argv[1]));
	server_addr.sin_addr.s_addr=INADDR_ANY;
	bindval=bind(sd,(struct sockaddr *) &server_addr,sizeof(server_addr));

	if(bindval<0){
		printf("Binding failure");
		exit(1);
	}

	listen(sd,backlog);

	clilength=sizeof(client_addr);

	ssd=accept(sd,(struct sockaddr *) &client_addr,&clilength);

	if(ssd<0){
		printf("Error entering in opening a slave socket\n");
		exit(1);
	}

	bzero(buff,256);

	read_flag=read(ssd,buff,255);

	if(read_flag<0){
		printf("Error in reading\n");
	}

	if(!strcmp(buff, "quit"))
	{
	  printf("Server quitting..\n");
	  int i = 1;
	  send(ssd, &i, sizeof(int), 0);
	  exit(0);
	}

	write(ssd,buff,strlen(buff));

	return 0;
}*/
