DEBUG ?= 0

CC			=	gcc
CFLAGS		=	-Werror -Wpedantic -Wall -Wextra -fPIC -std=c99 -D_DEFAULT_SOURCE
CPPFLAGS	=	-Iinclude

AR				=	ar
LDFLAGS			=	-shared 
LIBNAME			=	brkman

RM				=	rm
RM_OPTS			= 	-f -v 	

CTAGS			=	ctags
CTAGS_OPTS		=	-R --sort=yes 
CTAGS_TARGET	=	.

SRC		=	src
INC		=	include
TARGET	=	brkman
SRCS	=	$(wildcard $(SRC)/*.c)
INCS	=	$(wildcard $(INC)/*.h)
OBJS	=	$(SRCS:.c=.o)

TEST_SRC	= test
TEST_INC	= test
TEST_SRCS	= $(wildcard $(TEST_SRC)/*.c)
TEST_INCS	= $(wildcard $(TEST_INC)/*.h)
TEST_OBJS	= $(TEST_SRCS:.c=.o)
TEST_PROG	= $(TEST_SRC)/test_prog.out

CLANG_TIDY			=	clang-tidy
CLANG_TIDY_FILE		=	.clang-tidy
CLANG_FORMAT		=	clang-format
CLANG_FORMAT_FILE	=	.clang-format

ifeq ($(DEBUG), 1)
	CFLAGS		+= -g -fno-omit-frame-pointer -O0 -fsanitize=address -DDEBUG
	CPPFLAGS	+= -g
endif

.PHONY: all cleanbuild clean tidy format build-tests tags

all: lib$(LIBNAME).so lib$(LIBNAME).a

tidy:
	@$(CLANG_TIDY) $(SRCS) $(INCS) $(TEST_SRCS) $(TEST_INCS) -- -I$(INC) -I$(TEST_INC)

format:
	@$(CLANG_FORMAT) -i $(SRCS) $(INCS) $(TEST_SRCS) $(TEST_INCS)

%.o: %.c
	@$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

lib$(LIBNAME).so: $(OBJS)
	@$(CC) $(LDFLAGS) -o $@ $^

lib$(LIBNAME).a: $(OBJS)
	@$(AR) rcs $@ $^

build-tests: all
	@$(CC) $(CFLAGS) $(CPPFLAGS) -o $(TEST_PROG) $(TEST_SRCS) $(TEST_INCS) -L. -lbrkman

tags:
	@$(CTAGS) $(CTAGS_OPTS) $(CTAGS_TARGET)

clean:
	@$(RM) $(RM_OPTS) $(SRC)/*.o *.so *.a $(TEST_SRC)/*.o $(TEST_PROG) 
