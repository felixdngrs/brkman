DEBUG ?= 0

CC			=	gcc
CFLAGS		=	-Werror -Wpedantic -Wall -Wextra -fPIC -std=c99 -D_DEFAULT_SOURCE
CPPFLAGS	=	-Iinclude

AR				=	ar
LDFLAGS			=	-shared 
LIBNAME			=	brkman

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
TEST_PROG	= $(TEST_SRC)/test_prog

CLANG_TIDY			=	clang-tidy
CLANG_TIDY_FILE		=	.clang-tidy
CLANG_FORMAT		=	clang-format
CLANG_FORMAT_FILE	=	.clang-format

ifeq ($(DEBUG), 1)
	CFLAGS		+= -g -DDEBUG
	CPPFLAGS	+= -g
endif

.PHONY: all cleanbuild clean tidy format build-tests

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

build-tests:
	@$(CC) $(CFLAGS) $(CPPFLAGS) -o $(TEST_PROG) $(TEST_SRCS) $(TEST_INCS) -L. -lbrkman

clean:
	@rm $(SRC)/*.o *.so *.a $(TEST_SRC)/*.o $(TEST_PROG) 
