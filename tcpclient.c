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
#define TIME_TO_PUSH 20

int max(int a,int b){
	return b;
}

int main(int argc, char *argv[]){

	int csock,port,conflag,n;
	char receivebuffer[256],sendbuffer[256];
	struct sockaddr_in server_addr;
	struct hostent *server_name;
	int choice,choice1,status;
	int gasValue = 0;
	int electricityValue = 0;
	int waterValue = 0;
	int clientId =1001;

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

	int timer = 0;
	int test=0;

	/*------Test Start------*/
	time_t starttime,curtime;
	time(&starttime);

	while(1){

		time(&curtime);
		printf("difference time is %g",difftime(curtime,starttime));
		if(difftime(curtime,starttime)>=TIME_TO_PUSH){ printf("while 1 time complete"); time(&starttime); test=1;}

		printf("Enter a choice:\n1- Activities\n2- Generate a Report\n3- quit\n");

		scanf("%d",&choice);


		switch(choice)
		{
			case 1:

			 printf("Activities\n");

			 while(1)
			 {

				 time(&curtime);
				 printf("difference time is %g",difftime(curtime,starttime));
				 if(difftime(curtime,starttime)>=TIME_TO_PUSH){ printf("while 2 time complete"); time(&starttime); timer=1;}

			  printf("Enter a choice:\n1- Bathing\n2- Washing Machine\n3- Dish Washing\n4- Lighting\n5- Air Conditioner\n"
					  "6- Electronic Devices\n7- Cooking\n8- Water Heater\n9- Back\n");
			  scanf("%d",&choice1);



			  switch(choice1)
				{

				case 1:

				  printf("Bathing\n");
				  waterValue = waterValue + 30;

				  break;

				case 2:

				  printf("Washing\n");
				  waterValue = waterValue + 10;

				  break;

				case 3:

				  printf("Dish Washing\n");
				  waterValue = waterValue + 20;

				  break;

				case 4:

				  printf("Lighting\n");
				  electricityValue = electricityValue + 60;

				  break;

				case 5:

				  printf("Air Conditioner\n");
				  electricityValue = electricityValue + 30;

				  break;

				case 6:

				  printf("Electronic Devices\n");
				  electricityValue = electricityValue + 10;

				  break;

				case 7:

				  printf("Cooking\n");
				  gasValue = gasValue + 40;

				  break;

				case 8:

				  printf("Water Heater\n");
				  gasValue = gasValue + 20;

				  break;

				case 9:

				  printf("Back\n");
				  break;

				}
			  printf("Timer = %d",timer);
			  if(timer){
				char *pts;			/* pointer to time string	*/
				time_t	now;			/* current time			*/
				char	*ctime();

				(void) time(&now);
				pts = ctime(&now);

				strftime(pts,strlen(pts),"%m/%d/%Y %H:%M:%S %a",localtime(&now));
				sprintf(sendbuffer,"%d%c%d%c%d%c%s%c%d",waterValue,'|',gasValue,'|',electricityValue,'|',pts,'|',clientId);
				send(csock, sendbuffer,sizeof(sendbuffer),0);
				bzero(sendbuffer,sizeof(sendbuffer));
				recv(csock, &receivebuffer, sizeof(receivebuffer), 0);
				printf("The buffer in Client is:: %s\n",receivebuffer);

				waterValue=gasValue=electricityValue=0;
				timer=0;
				time(&starttime);

			  }

			  if(test){

				  char *pts;			/* pointer to time string	*/
				  time_t	now;			/* current time			*/
				  char	*ctime();

				  (void) time(&now);
				  pts = ctime(&now);

				  strftime(pts,strlen(pts),"%m/%d/%Y %H:%M:%S %a",localtime(&now));
				  sprintf(sendbuffer,"%d%c%d%c%d%c%s%c%d",waterValue,'|',gasValue,'|',electricityValue,'|',pts,'|',clientId);
				  send(csock, sendbuffer,sizeof(sendbuffer),0);
				  bzero(sendbuffer,sizeof(sendbuffer));
				  recv(csock, &receivebuffer, sizeof(receivebuffer), 0);
				  printf("The buffer in Client is:: %s\n",receivebuffer);

				  waterValue=gasValue=electricityValue=0;
				  time(&starttime);

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


				  switch(choice1)
					{

				  	  case 1:

				  	  printf("Text Report\n");
				  	  strcpy(sendbuffer, "text");
				  	  send(csock, sendbuffer, 100, 0);
				  	  break;

				  	  case 2:

					  printf("Graph Report\n");
					  strcpy(sendbuffer, "graph");
					  send(csock, sendbuffer, 100, 0);
					  break;

				  	  case 3:

					  printf("Back\n");
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
				  printf("Server closed\nQuitting..\n");
				  exit(0);
			  }

			  printf("Server failed to close connection\n");

		}

	}


	/*------Test End------*/

	close(csock);

	return 0;
}

