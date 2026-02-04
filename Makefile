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

CLANG_TIDY			=	clang-tidy
CLANG_TIDY_FILE		=	.clang-tidy
CLANG_FORMAT		=	clang-format
CLANG_FORMAT_FILE	=	.clang-format


.PHONY: all cleanbuild clean tidy format

all: lib$(LIBNAME).so lib$(LIBNAME).a

tidy:
	@$(CLANG_TIDY) $(SRCS) $(INCS) -- -Iinclude

format:
	@$(CLANG_FORMAT) -i $(SRCS) $(INCS)

%.o: %.c
	@$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

lib$(LIBNAME).so: $(OBJS)
	@$(CC) $(LDFLAGS) -o $@ $^

lib$(LIBNAME).a: $(OBJS)
	@$(AR) rcs $@ $^


clean:
	@rm $(SRC)/*.o *.so *.a