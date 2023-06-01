EXEC = server_test
ARGS =

CC = gcc
CFLAGS = -g -Wall -Wno-int-conversion -Wno-unused-parameter -Wno-unused-function -Wextra -std=c99
# -std=c99
# -Wall -Wno-int-conversion -Wno-unused-parameter -Wno-unused-function -fno-inline -O0 -pthread -g -ggdb -static-libasan -Wextra -O2 -ansi -std=c99

CFLAGS += -D HTTP=1 
# -D HTTP=1
# CFLAGS += -D PARSER=1 -D DEBUG
# -D DEBUG
# -D FORCE_IDENTITY=1

IGNORE = tst.c
OUTDIR = ./bin
DATADIR = ./data
SUBDIR = utils src
DIR_OBJ = ./bin

export LD_LIBRARY_PATH=$(dir $(abspath $(firstword $(MAKEFILE_LIST))))/lib
INCS = $(wildcard *.h $(foreach fd, $(SUBDIR), $(fd)/*.h))
SRCS = $(wildcard *.c $(foreach fd, $(SUBDIR), $(fd)/*.c))
NODIR_SRC = $(notdir $(SRCS))
OBJS = $(addprefix $(DIR_OBJ)/, $(SRCS:c=o)) # obj/xxx.o obj/folder/xxx .o

INC_DIRS = -I./lib
# -I./ $(addprefix -I, $(SUBDIR))

# gcc -o ./bin/http_parse ./bin/*/*.o -L./lib -lrequest -lmagic -Wall -std=c99 -D TST=0
LIBS = -L./lib -lrequest -lz

PHONY := $(EXEC)

$(EXEC): $(OBJS)
	@mkdir -p $(OUTDIR)
	$(CC) $(CFLAGS) -o $(OUTDIR)/$@ $(OBJS) $(LIBS)

$(DIR_OBJ)/utils/thirdpart.o: utils/thirdpart.c $(DIR_OBJ)/utils/socket_1.o
	@mkdir -p $(DIR_OBJ)/utils
	$(CC) $(CFLAGS) $(INC_DIRS) -c -o $@ $<

$(DIR_OBJ)/%.o: %.c $(INCS)
	@mkdir -p $(OUTDIR)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INC_DIRS) -c -o $@ $<

PHONY += clean
clean:
	@reset
	rm -rf $(OUTDIR)/* $(DATADIR)/* $(DIR_OBJ)/*

PHONY += echoes
echoes:
	@echo "INC files: $(INCS)"
	@echo "SRC files: $(SRCS)"
	@echo "OBJ files: $(OBJS)"
	@echo "LIB files: $(LIBS)"
	@echo "INC DIR: $(INC_DIRS)"
	@echo "LIB DIR: $(LIB_DIRS)"

.PHONY = $(PHONY)

dev: clean echoes $(EXEC) run

tests: clean $(EXEC)
	@echo "Running tests..."
	@./tests.sh

run: $(EXEC)
	# @reset
	-pkill -9 -f $(EXEC)
	./$(OUTDIR)/$(EXEC) $(ARGS)

bt: $(EXEC)
	@reset
	gdb -batch -ex "run" -ex "bt" --args ./$(OUTDIR)/$(EXEC) $(ARGS)

leaks:
	$(MAKE) clean
	$(MAKE) CFLAGS="$(CFLAGS) -D LEAK_CHECK=1" $(EXEC)
	valgrind --leak-check=full \
		--show-leak-kinds=all \
		--track-origins=yes \
		--log-file=valgrind_out.txt -s ./$(OUTDIR)/$(EXEC) $(ARGS)

abnf:
	gcc -g -o obj/manip.o -c utils/manip.c
	gcc -g -o obj/abnf.o -c utils/abnf.c
	gcc -g -o obj/abnf_test.o obj/abnf.o obj/manip.o
	./obj/abnf_test.o


rendu:
	zip -r9 "rendus/LEPRAT_MONCORGE_MOUSSU_$$(date +"%Y-%m-%-d-%H-%M-%S").zip" . -x *.git* *.vscode* Docs2022/\* allrfc.abnf rendus/\* tests/**\*

autoreload:
	./autoreload.sh

keepalive:
	curl -v http://localhost:8000/www.toto.com/index.html  --next http://localhost:8000/www.toto.com/index.html  --next http://localhost:8000/www.toto.com/index.html 2>&1 | grep "Connection "

tsth:
	/bin/echo -ne 'GET /www.toto.com/index.html HTTP/1.1\r\nHost: localhost:8000\r\nConnection: keep-alive\r\nUser-Agent: Mozilla/5.0 \r\n\r\n' | ncat -C --hex-dump out localhost 8000

range:
	curl -v -r 0-199 "http://localhost:8000/o.com/Mask%20Off%E2%9D%A4%20%5BLhg2dMh49YA%5D.webm" -o ooo

gzip:
	curl -v --compressed http://localhost:8000/www.toto.com/index.html

tstz:
	/bin/echo -ne 'GET /www.toto.com/index.html HTTP/1.1\r\nHost: localhost:8000\r\nConnection: keep-alive\r\nUser-Agent: Mozilla/5.0 \r\nAccept-Encoding: gzip\r\n\r\n' | ncat -C --hex-dump out localhost 8000

gzip2:
	curl 'http://localhost:8000/www.toto.com/index.png' --compressed -H 'User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/114.0' -H 'Accept: image/avif,image/webp,*/*' -H 'Accept-Language: en-US,en;q=0.5' -H 'Accept-Encoding: gzip, deflate, br' -H 'Connection: keep-alive' -H 'Referer: http://localhost:8000/www.toto.com/' -H 'Sec-Fetch-Dest: image' -H 'Sec-Fetch-Mode: no-cors' -H 'Sec-Fetch-Site: same-origin' -H 'Pragma: no-cache' -H 'Cache-Control: no-cache'
# /bin/echo -ne 'GET /www.toto.com/index.html HTTP/1.1\r\nHost: localhost:8000\r\nConnection: keep-alive\r\nUser-Agent: Mozilla/5.0 \r\nAccept-Encoding: gzip\r\n\r\n' | ncat -C --hex-dump out localhost 8000
php:
	curl -v 'http://localhost:8000/post.php?name=titi&email=toto' -X POST -H 'User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/114.0' -H 'Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8' -H 'Accept-Language: en-US,en;q=0.5' -H 'Accept-Encoding: gzip, deflate, br' -H 'Referer: http://localhost:8000/post.php?name=titi&email=toto' -H 'Origin: http://localhost:8000' -H 'Connection: keep-alive' -H 'Upgrade-Insecure-Requests: 1' -H 'Sec-Fetch-Dest: document' -H 'Sec-Fetch-Mode: navigate' -H 'Sec-Fetch-Site: same-origin' -H 'Sec-Fetch-User: ?1' -H 'Content-Type: application/x-www-form-urlencoded' -H 'Pragma: no-cache' -H 'Cache-Control: no-cache' --data-raw 'name=trkre&email=zer'

php2:
	/bin/echo -e "GET /post.php HTTP/1.1\r\nHost: localhost:8000\r\nUser-Agent: M0\r\nAccept: */*\r\nAccept-Language: en-US,en;q=0.5\r\nAccept-Encoding: gzip, deflate, br\r\nConnection: keep-alive\r\n\r\n"  | ncat -C --hex-dump out localhost 8000
