EXEC = server_test
ARGS = 

CC = gcc
CFLAGS = -g -Wall -Wno-int-conversion -Wno-unused-parameter -Wno-unused-function -Wextra -std=c99 
# -std=c99
# -Wall -Wno-int-conversion -Wno-unused-parameter -Wno-unused-function -fno-inline -O0 -pthread -g -ggdb -static-libasan -Wextra -O2 -ansi -std=c99 

CFLAGS += -D HTTP=1 -D FORCE_IDENTITY=1
# -D HTTP=1
# -D PARSER=1
# -D DEBUG

IGNORE = tst.c
OUTDIR = ./bin
DATADIR = ./data
SUBDIR = utils src
DIR_OBJ = ./bin

# LD_LIBRARY_PATH=$(dir $(abspath $(firstword $(MAKEFILE_LIST))))/lib
INCS = $(wildcard *.h $(foreach fd, $(SUBDIR), $(fd)/*.h))
SRCS = $(wildcard *.c $(foreach fd, $(SUBDIR), $(fd)/*.c))
NODIR_SRC = $(notdir $(SRCS))
OBJS = $(addprefix $(DIR_OBJ)/, $(SRCS:c=o)) # obj/xxx.o obj/folder/xxx .o

INC_DIRS = -I./lib
# -I./ $(addprefix -I, $(SUBDIR))

# gcc -o ./bin/http_parse ./bin/*/*.o -L./lib -lrequest -lmagic -Wall -std=c99 -D TST=0
LIBS = -L./lib -lrequest

PHONY := $(EXEC)
$(EXEC): $(OBJS)
	@mkdir -p $(OUTDIR)
	$(CC) $(CFLAGS) -o $(OUTDIR)/$@ $(OBJS) $(LIBS)

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

leaks: $(EXEC)
	valgrind --leak-check=full \
		--show-leak-kinds=all \
		--track-origins=yes \
		--log-file=valgrind_out.txt -s ./$(OUTDIR)/$(EXEC) $(ARGS)

abnf:
	gcc -g -o obj/manip.o -c utils/manip.c
	gcc -g -o obj/abnf.o -c utils/abnf.c
	gcc -g -o obj/abnf_test.o obj/abnf.o obj/manip.o
	./obj/abnf_test.o

parse:


rendu:
	zip -r9 "rendus/LEPRAT_MONCORGE_MOUSSU_$$(date +"%Y-%m-%-d-%H-%M-%S").zip" . -x *.git* *.vscode* Docs2022/\* allrfc.abnf rendus/\* tests/**\*

autoreload:
	./autoreload.sh

keepalive:
	curl -v http://localhost:8000/www.toto.com/index.html  --next http://localhost:8000/www.toto.com/index.html  --next http://localhost:8000/www.toto.com/index.html 2>&1 | grep "Connection "

tsth:
	/bin/echo -ne 'GET /www.toto.com/index.html HTTP/1.1\r\nHost: localhost:8000\r\nConnection: keep-alive\r\nUser-Agent: Mozilla/5.0 \r\n\r\n' | ncat -C --hex-dump out localhost 8000

tsti:
	/bin/echo -ne 'GET / HTTP/1.1\r\nHost: flolep.fr\r\nConnection: keep-alive\r\nUser-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/110.0.0.0 Safari/537.36\r\nAccept-Encoding: deflate\r\n\r\n' | ncat -C -o out flolep.fr 80

tstt:
	curl -v http://flolep.fr/ --next http://flolep.fr/ 2>&1 | grep "Connection "
