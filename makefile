CC	=	gcc
CCOPTS	=	-g
PTHREAD	=	-pthread

TARGETS	=	tcpserver	tcpclient				

all	:	$(TARGETS) 		

tcpserver	:	tcpserver.c	gnuplot_i.c
	$(CC) 	$(PTHREAD)	$(CCOPTS) $^	-o	$@

tcpclient	:	tcpclient.c
	$(CC)	$(PTHREAD)	$(CCOPTS) $^	-o	$@	
	
clean	:	
	rm	-f	$(TARGETS)	*.o 
    