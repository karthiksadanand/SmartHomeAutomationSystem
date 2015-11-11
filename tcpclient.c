#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include<pthread.h>
#define TIME_TO_PUSH 20

/*for getting file size using stat()*/
#include<sys/stat.h>

/*for sendfile()*/
#include<sys/sendfile.h>

/*for O_RDONLY*/
#include<fcntl.h>

int gasValue = 0;
int electricityValue = 0;
int waterValue = 0;
int clientId =1001;
char receivebuffer[256],sendbuffer[256];
int csock;


void push_to_client(){

	//code to push to client
	char *pts;			/* pointer to time string	*/
	time_t	now;			/* current time			*/
	char	*ctime();

	while(1){

	sleep(TIME_TO_PUSH);

	(void) time(&now);
	pts = ctime(&now);

	strftime(pts,strlen(pts),"%m/%d/%Y %H:%M:%S %a",localtime(&now));
	sprintf(sendbuffer,"%s%c%d%c%d%c%d%c%s%c%d","usage",'|',waterValue,'|',gasValue,'|',electricityValue,'|',pts,'|',clientId);

	send(csock, sendbuffer,sizeof(sendbuffer),0);
	bzero(receivebuffer,sizeof(receivebuffer));
	recv(csock, &receivebuffer,sizeof(receivebuffer),0);
	gasValue = 0;
	electricityValue = 0;
	waterValue = 0;
	printf("%s\n",receivebuffer);

	}
}

int main(int argc, char *argv[]){

	int port,conflag,size;
	char *fileptr;
	char filename[20];
	struct sockaddr_in server_addr;
	struct hostent *server_name;
	int choice,choice1,status;
	/*int gasValue = 0;
	int electricityValue = 0;
	int waterValue = 0;
	int clientId =1001;*/
	int filehandle;
	int i = 1;

	csock=socket(AF_INET,SOCK_STREAM,0);

	if(csock==-1){
		printf("socket failure\n");
	}

	server_name=gethostbyname(argv[1]);

	if(server_name==NULL){
		printf("No host found");
	}

	port=atoi(argv[2]);

	bzero((char *)&server_addr, sizeof(server_addr));
	server_addr.sin_family=AF_INET;
	server_addr.sin_port=htons(port);
	bcopy((char *)server_name->h_addr,(char *)&server_addr.sin_addr.s_addr,server_name->h_length);

	conflag=connect(csock,(struct sockaddr *)&server_addr,sizeof(server_addr));

	if(conflag<0){
		printf("Error in connecting");
	}

	/*int timer = 0;*/

	/*------Test Start------*/
	pthread_attr_t ta;
	pthread_t th;
	(void) pthread_attr_init(&ta);
	(void) pthread_attr_setdetachstate(&ta, PTHREAD_CREATE_DETACHED);
	if (pthread_create(&th, &ta, (void * (*)(void *))push_to_client, NULL) < 0)
		printf("Thread error \n");

	while(1){

		/*time(&curtime);
		printf("difference time is %g",difftime(curtime,starttime));
		if(difftime(curtime,starttime)>=TIME_TO_PUSH){ printf("while 1 time complete"); time(&starttime); timer=1;}*/

		printf("Enter a choice:\n1- Activities\n2- Generate a Report\n3- quit\n");

		scanf("%d",&choice);

		switch(choice)
		{
			case 1:

			 printf("Activities\n");

			 while(1)
			 {

				 /*time(&curtime);
				 printf("difference time is %g",difftime(curtime,starttime));
				 if(difftime(curtime,starttime)>=TIME_TO_PUSH){ printf("while 2 time complete"); time(&starttime); timer=1;}*/

			  printf("Enter a choice:\n1- Bathing\n2- Washing Machine\n3- Dish Washing\n4- Lighting\n5- Air Conditioner\n"
					  "6- Electronic Devices\n7- Cooking\n8- Water Heater\n9- Back\n");
			  scanf("%d",&choice1);

			  int test=0;

			  switch(choice1)
				{

				case 1:

				  printf("Bathing Used\n");
				  waterValue = waterValue + 30;

				  break;

				case 2:

				  printf("Washing Used\n");
				  waterValue = waterValue + 10;

				  break;

				case 3:

				  printf("Dish Washing Used\n");
				  waterValue = waterValue + 20;

				  break;

				case 4:

				  printf("Lighting Used\n");
				  electricityValue = electricityValue + 60;

				  break;

				case 5:

				  printf("Air Conditioner Used\n");
				  electricityValue = electricityValue + 30;

				  break;

				case 6:

				  printf("Electronic Devices Used\n");
				  electricityValue = electricityValue + 10;

				  break;

				case 7:

				  printf("Cooking Used\n");
				  gasValue = gasValue + 40;

				  break;

				case 8:

				  printf("Water Heater Used\n");
				  gasValue = gasValue + 20;
				  break;

				case 9:

				  printf("Back\n");
				  test=1;
				  break;

				}

			  /*if(timer){

				char *pts;			 pointer to time string
				time_t	now;			 current time
				char	*ctime();

				(void) time(&now);
				pts = ctime(&now);

				strftime(pts,strlen(pts),"%m/%d/%Y %H:%M:%S %a",localtime(&now));
				sprintf(sendbuffer,"%s%c%d%c%d%c%d%c%s%c%d","usage",'|',waterValue,'|',gasValue,'|',electricityValue,'|',pts,'|',clientId);

				send(csock, sendbuffer,sizeof(sendbuffer),0);
				bzero(receivebuffer,sizeof(receivebuffer));
				recv(csock, &receivebuffer,sizeof(receivebuffer),0);
				gasValue = 0;
				electricityValue = 0;
				waterValue = 0;
				printf("%s\n",receivebuffer);

			  }*/

			  if(test){
				  test = 0;
				  break;
			  }

			}

			  break;

			case 2:

			  printf("Generate a Report\n");

			  while(1)
				 {

				  printf("Enter a choice:\n1- Text Report\n2- Graph Report\n3- Back\n");
				  scanf("%d",&choice1);

				  int test=0;

				  switch(choice1)
					{

				  	  case 1:

				  	  sprintf(sendbuffer,"%s%c%d","text",'|',clientId);
					  send(csock, sendbuffer, 100, 0);

					  bzero(receivebuffer,sizeof(receivebuffer));
					  recv(csock, receivebuffer, sizeof(receivebuffer), 0);

					  strcpy(filename,receivebuffer);

					  send(csock, receivebuffer, 100, 0);

					  recv(csock, &size, sizeof(int), 0);

					  if(!size)
					  {
						  printf("No such file on the remote directory\n\n");
						  break;
					  }

					  fileptr = malloc(size);
					  recv(csock, fileptr, size, 0);

					  while(1)
					  {
						  filehandle = open(filename, O_CREAT | O_EXCL | O_WRONLY, 0666);
						  if(filehandle == -1)
						  {
							  sprintf(filename + strlen(filename), "%d", i);//needed only if same directory is used for both server and client
						  }
						  else break;
					  }

					  write(filehandle, fileptr, size);
					  close(filehandle);

					  printf("The file %s has been downloaded\n",filename);

					  break;

				  	  case 2:

					  sprintf(sendbuffer,"%s%c%d","graph",'|',clientId);
					  send(csock, sendbuffer, 100, 0);

					  bzero(receivebuffer,sizeof(receivebuffer));
					  recv(csock, receivebuffer, sizeof(receivebuffer), 0);

					  strcpy(filename,receivebuffer);

					  send(csock, receivebuffer, 100, 0);

					  recv(csock, &size, sizeof(int), 0);

					  if(!size)
					  {
						  printf("No such file on the remote directory\n\n");
						  break;
					  }

					  fileptr = malloc(size);
					  recv(csock, fileptr, size, 0);

					  while(1)
					  {
						  filehandle = open(filename, O_CREAT | O_EXCL | O_WRONLY, 0666);
						  if(filehandle == -1)
						  {
							  sprintf(filename + strlen(filename), "%d", i);//needed only if same directory is used for both server and client
						  }
						  else break;
					  }

					  write(filehandle, fileptr, size);
					  close(filehandle);

					  printf("The file %s has been downloaded\n",filename);

					  break;

				  	  case 3:

					  printf("Back\n");
					  test = 1;
					  break;

					}

					if(test){
					  test = 0;
					  break;
					}
				 }

			  break;

			case 3:

			  strcpy(sendbuffer, "quit");
			  send(csock, sendbuffer, 100, 0);
			  recv(csock, &status, 100, 0);

			  if(status){
				  printf("Client is closed\nQuitting..\n");
				  exit(0);
			  }

			  printf("Client failed to close connection\n");

		}

	}
	pthread_cancel(th);


	/*------Test End------*/

	close(csock);

	return 0;
}

