export LD_LIBRARY_PATH=$PWD/../libparser/:$PWD/../librequest-0.5/

all: 	
	gcc -I ../libparser -I ../librequest-0.5/api  -L ../libparser -L ../librequest-0.5 -o server server.c -lparser -lrequest 
