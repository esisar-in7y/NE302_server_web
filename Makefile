AUTEURS := Moncorge_Leprat
# PLUS_FLAGS = -fsanitize=address,leak,undefined -Wall -Wextra -Wshadow -Wdouble-promotion -Wundef -Wconversion
ARGS ?= -pas
CC = gcc
EXEC = http_parse
DEBUG = yes
CFLAGS = -Wall -Wextra -O2 -ansi -std=c99  
#  -pedantic   -fsanitize=address,leak,undefined
STATICDEFINE = 
# TEST
lib = m

OBJDIR = obj
SRCDIR = src/utils
INCDIR = includes
LIBDIR = lib
RESULTDIR = out
DATADIR = data
MAIN = main
# MAIN = test

D ?= 

SRCS=$(wildcard $(SRCDIR)/*.c)
OBJS= $(SRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
HEADER =$(wildcard $(INCDIR)/*.h)

CFLAGS += $(foreach headerdir,$(INCDIR),-I$(headerdir)) 
CFLAGS += $(foreach librarydir,$(LIBDIR),-L$(librarydir))
CFLAGS += $(foreach library,$(lib),-l$(library))
CFLAGS += $(foreach defined,$(D),-D$(defined))

ifeq ($(DEBUG),yes)
	CFLAGS += -g
	CFLAGS += $(foreach defined,$(STATICDEFINE),-D$(defined))
endif


$(EXEC):$(OBJS) $(HEADER)
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) $(OBJS) -o $@


$(OBJDIR)/%.o: $(SRCDIR)/%.c $(HEADER)
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -o $@ -c $< 

.PHONY: clean run create debug tests doc filespas files pasapas test-cli

dev: clean $(EXEC) run

clean:
	reset
	@rm -rf $(OBJDIR) $(EXEC) $(RESULTDIR)

run: $(EXEC)
	@./$(EXEC) $(ARGS)

valgrind: $(EXEC)
	@mkdir -p $(RESULTDIR)
	valgrind --track-origins=yes --leak-check=full ./$(EXEC) tests/labels.s $(RESULTDIR)/bin $(RESULTDIR)/stdout -pas

create: $(EXEC)
	@mkdir -p {$(DATADIR),$(SRCDIR),$(INCDIR),$(LIBDIR),$(LIBDIR),$(RESULTDIR)}

tests: $(EXEC)
	@python3 test.py -v

CLITMP := /tmp/emul-mips-test
test-cli: $(EXEC)
	@ [ -e ./$(EXEC) ] \
	  || echo "error: emul-mips does not exist!"; \
	touch $(CLITMP).in; \
	rm -f $(CLITMP).out1 $(CLITMP).out2; \
	./$(EXEC) $(CLITMP).in $(CLITMP).out1 $(CLITMP).out2 \
	  || echo "error: emul-mips in automatic mode returned $$?!"; \
	[ -e $(CLITMP).out1 ] \
	  || echo "error: assembled output file does not exist!"; \
	[ -e $(CLITMP).out2 ] \
	  || echo "error: final state output file does not exist!"; \
	rm -f $(CLITMP).out1 $(CLITMP).out2

tar: clean
	@ dir=$$(basename "$$PWD") && cd .. && touch $(AUTEURS).tgz && tar --exclude='*.pdf' --exclude='rendus/*' --exclude='docs/*' -czvf $(AUTEURS).tgz "$$dir"
	@ echo "==="; echo "Created ../$(AUTEURS).tgz"

doc:
	doxygen doxy_config
