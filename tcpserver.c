#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<string.h>
#include<errno.h>
#include "gnuplot_i.h"
#include<pthread.h>

/*for getting file size using stat()*/
#include<sys/stat.h>

/*for sendfile()*/
#include<sys/sendfile.h>

/*for O_RDONLY*/
#include<fcntl.h>

void *client_req(void*);
float conservation(int value,int threshold);
float consumption(int value,int threshold);

pthread_mutex_t usage_lock;

int w_val = 100;
int g_val = 80;
int e_val = 120;
char firstname[10]= {0};


int main(int argc, char *argv[]){

	int sd, bindval,csock;
	struct sockaddr_in client_addr, server_addr;
	socklen_t clilength;
	int backlog=5;

	fd_set rfds;
    FD_ZERO(&rfds);
    int nfds = getdtablesize();
	pthread_t thread1;

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

    fflush(stdout);

	while(1){

		FD_SET(sd, &rfds);

		if (select(nfds, &rfds, (fd_set *)0, (fd_set *)0,(struct timeval *)0) < 0){
			printf("select() Failure\n");
	    	exit(1);
		}

		if(FD_ISSET(sd,&rfds))
		{

			csock=accept(sd,(struct sockaddr *) &client_addr,&clilength);

			if(csock<0)
				if (errno == EINTR)
						continue;

			printf("%s client with port number %d is connected to  server %s with port number %d \n",inet_ntoa(client_addr.sin_addr),htons(client_addr.sin_port),inet_ntoa(server_addr.sin_addr),htons(server_addr.sin_port));
			pthread_create(&thread1, NULL, (void *(*)(void*))client_req, (void *)&csock);
		}

	}

	pthread_join(thread1,NULL);
	close(sd);

	return 0;
}

void *client_req(void* arg)
{
	int read_flag=0;
	char buff[256];
	struct sockaddr_in client_addr, server_addr;
	struct stat obj;
	int filehandle,size;
	FILE *file ;
	FILE *newFile;

	int csock = *(int*)arg;

	while(1){

		bzero(buff,sizeof(buff));
		read_flag=read(csock,buff,sizeof(buff));

		if(read_flag<0){
			printf("Error in reading\n");
		}

		if(!strcmp(buff, "quit"))
		{
		  //printf("Server quitting..\n");
		  int i = 1;
		  send(csock, &i, sizeof(int), 0);
		  //exit(0);
		}
		if(!strncmp(buff,"usage",5))
		{
			char *token;
			token=strtok(buff,"|");
			int flag=0,client_id=0, water_val=0,gas_val=0,elec_val=0;
			char time_stamp[200],cid[20];
			bzero(time_stamp,200);

			 while(token!=NULL)
			 {
				 if(1==flag)
				 {
					 water_val=atoi(token);
				 }
				 else if(2==flag)
				 {
					 gas_val= atoi(token);
				 }
				 else if(3==flag)
				 {
					 elec_val= atoi(token);
				 }
				 else if(4==flag)
				 {
					 strcpy(time_stamp,token);
				 }
				 else if(5==flag)
				 {
					 client_id= atoi(token);
					 strcpy(cid,token);

				 }
				 flag++;
				 token=strtok(NULL,"|");
			 }

			char str[20];
			strcpy(str,"User ");
			strcat(str,cid);
			strcat(str,".txt");
			file = fopen(str,"a+");

			if (file == NULL)
			{
			  printf("Error opening file!\n");
			  exit(1);
			}

			fscanf(file,"%s",firstname);

			if(!strncmp(firstname,"water",5)) {
				fprintf(file,"%d\t%d\t%d\t\t%s\t\t%d\n",water_val,gas_val,elec_val,time_stamp,client_id);
			}else{
				fprintf(file,"water\tgas\telectricity\ttime_day\t\tclient_id\n");
				fprintf(file,"%d\t%d\t%d\t\t%s\t\t%d\n",water_val,gas_val,elec_val,time_stamp,client_id);
			}

			bzero(firstname,sizeof(firstname));
			bzero(buff,sizeof(buff));

			pthread_mutex_lock(&usage_lock);

			if(water_val > w_val) {
				sprintf(buff, "U have crossed threshold by %0.2f%%\n" , consumption(water_val,w_val));
			} else if(water_val < w_val){
				sprintf(buff, "Congratulations! U have conserved %0.2f%% water\n", conservation(water_val,w_val));
			} else {
				sprintf(buff,"Your current water usage is equal to threshold\n");
			}

			if(gas_val > g_val) {
				sprintf(buff, "%s\n U have crossed threshold by %0.2f%% \n",buff,consumption(gas_val,g_val));
			} else if(gas_val < g_val){
				sprintf(buff, "%s\nCongratulations!U have conserved %0.2f%% gas\n",buff, conservation(gas_val,g_val));
			} else {
				sprintf(buff,"%s\nYour current gas usage is equal to threshold\n",buff);
			}

			if(elec_val > e_val) {
				sprintf(buff, "%s\n U have crossed threshold %0.2f%%\n",buff,consumption(elec_val,e_val));
			} else if(elec_val < e_val){
				sprintf(buff, "%s\nCongratulations! U have conserved %0.2f%% electricity\n",buff, conservation(elec_val,e_val));
			} else {
				sprintf(buff,"%s\nYour current electricity usage is equal to threshold\n",buff);
			}

			pthread_mutex_unlock(&usage_lock);

			write(csock,buff,strlen(buff));

			fclose(file);
		}

		if(strncmp(buff,"text",4)==0)
		{
			char *token;
			token=strtok(buff,"|");
			int flag=0;
			char cid[20];

			while(token!=NULL)
			{
				if(flag==1)
				 {
					 strcpy(cid,token);
				 }
			 flag++;
			 token=strtok(NULL,"|");
			}

			char str[20];
			strcpy(str,"User ");
			strcat(str,cid);
			strcat(str,".txt");

			file=fopen(str,"r");

			if (file == NULL)
			{
			  printf("Error opening file!\n");
			  exit(1);
			}

			char str1[30] = {0};
			strcpy(str1,"User_");
			strcat(str1,cid);
			strcat(str1,".txt");

			strcpy(buff,str1);
			write(csock,buff,strlen(buff));

			read(csock,buff,sizeof(buff));

			char ch[150] = {0};

			newFile = fopen(str1, "w");

			if (newFile == NULL)
			{
				printf("Error opening file!\n");
				exit(1);
			}

			while (fgets(ch, 150, file) != NULL)
			{
				fputs(ch,newFile);
			}

			fclose(newFile);
			fclose(file);

			stat(str1, &obj);
			filehandle = open(str1, O_RDONLY);
			size = obj.st_size;

			if(filehandle == -1)
			  size = 0;
			send(csock, &size, sizeof(int), 0);
			if(size)
			  sendfile(csock, filehandle, NULL, size);
		}

		if(strncmp(buff,"graph",5)==0)
		{
			char *token;
			token=strtok(buff,"|");
			int flag=0,client_id=0;
			char cid[20];

			 while(token!=NULL)
			 {
				if(1==flag)
				 {
					 client_id= atoi(token);
					 strcpy(cid,token);
				 }
				 flag++;
				 token=strtok(NULL,"|");
			 }

			 char str[20];
			 strcpy(str,"User ");
			 strcat(str,cid);
			 strcat(str,".txt");

			 char str1[30] = {0};
			 strcpy(str1,"User_");
			 strcat(str1,cid);
			 strcat(str1,".png");

			 strcpy(buff,str1);
			 write(csock,buff,strlen(buff));

			 read(csock,buff,sizeof(buff));
			char timefmt[]="%m/%d/%Y %H:%M:%S";
			 gnuplot_ctrl * g = gnuplot_init();
			 gnuplot_cmd(g, "set terminal png");
			 gnuplot_cmd(g, "set output \"%s\"",str1);
			 gnuplot_cmd(g,"set xdata time");
			 gnuplot_cmd(g,"set timefmt \"%s\"",timefmt);
			 gnuplot_cmd(g,"set format x \"%s\"",timefmt);
			 gnuplot_cmd(g, "set xtics rotate # crucial line");
			 gnuplot_cmd(g, "set xlabel \"Duration\"");
			 gnuplot_cmd(g, "set ylabel \"Usage\"");
			 gnuplot_cmd(g, "set yrange [0:500]");
			 gnuplot_cmd(g, "plot \"%s\" using 4:1 with linespoints title columnhead,\\",str);
			 gnuplot_cmd(g, "\"%s\" using 4:2 with linespoints title columnhead,\\",str);
			 gnuplot_cmd(g, "\"%s\" using 4:3 with linespoints title columnhead",str);

			 gnuplot_close(g);

			 bzero(buff,256);
			 strcpy(buff," Graph Report Generated");

			 stat(str1, &obj);
			 filehandle = open(str1, O_RDONLY);
			 size = obj.st_size;

			 if(filehandle == -1)
			   size = 0;
			 send(csock, &size, sizeof(int), 0);

			 if(size)
			  sendfile(csock, filehandle, NULL, size);

		}
	}
}

float conservation(int value,int threshold) {

	float computedValue = threshold-value;
	computedValue = computedValue/threshold;
	computedValue = computedValue*100;
	return computedValue;

}

float consumption(int value,int threshold) {

	float computedValue = value-threshold;
	computedValue = computedValue/threshold;
	computedValue = computedValue*100;
	return computedValue;

}
