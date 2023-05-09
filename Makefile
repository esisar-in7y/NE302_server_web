# EXEC = http_parser
# ARGS = tests/testFile/test1.txt nombre
# ARGS = tests/tests.txt HTTP_name
EXEC = server_test

CC = gcc
CFLAGS = -fno-inline -O0 -pthread -g -ggdb -static-libasan -Wall -Wextra -O2 -ansi -std=c99 -Wno-int-conversion -D TST=0
# -D ABNF=1
# -D DEBUG

IGNORE = tst.c
OUTDIR = ./bin
DATADIR = ./data
SUBDIR = utils src
DIR_OBJ = ./obj

INCS = $(wildcard *.h $(foreach fd, $(SUBDIR), $(fd)/*.h))
SRCS = $(wildcard *.c $(foreach fd, $(SUBDIR), $(fd)/*.c))
NODIR_SRC = $(notdir $(SRCS))
OBJS = $(addprefix $(DIR_OBJ)/, $(SRCS:c=o)) # obj/xxx.o obj/folder/xxx .o
INC_DIRS = -I./ $(addprefix -I, $(SUBDIR))
LIBS =  -lmagic
LDFLAGS = 
INC_DIRS = -I./ $(addprefix -I, $(SUBDIR)) -I./api -I./request

LIB_DIRS = 

PHONY := $(EXEC)
$(EXEC): $(OBJS)
	@mkdir -p $(OUTDIR)
	$(CC) -o $(OUTDIR)/$@ $(OBJS) $(LIB_DIRS) $(LIBS) $(CFLAGS)



$(DIR_OBJ)/%.o: %.c $(INCS)
	@mkdir -p $(@D)
	$(CC) -o $@ $(CFLAGS) -c $< $(INC_DIRS)

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
	@reset
	# ./$(OUTDIR)/$(EXEC) $(ARGS)
	./$(OUTDIR)/$(EXEC)

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

rendu:
	zip -r9 "rendus/LEPRAT_MONCORGE_MOUSSU_$$(date +"%Y-%m-%-d-%H-%M-%S").zip" . -x *.git* *.vscode* Docs2022/\* allrfc.abnf rendus/\* tests/**\*
