ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

NAME		= libft_malloc_$(HOSTTYPE).so
SYMLINK		= libft_malloc.so

CC			= gcc
CFLAGS		= -Wall -Wextra -Werror -fPIC -g
# CFLAGS		+= -O0 -fno-omit-frame-pointer
LDFLAGS		= -shared

SRCDIR		= src
OBJDIR		= obj
INCDIR		= inc 

SRC_FILES	= malloc.c \
			  show.c \
			  utils/bins.c \
			  utils/block.c \
			  utils/page.c \
			  utils/utils.c

OBJS		= $(addprefix $(OBJDIR)/, $(SRC_FILES:.c=.o))

all: $(NAME)

$(NAME): $(OBJS)
	@echo "Linking $(NAME)..."
	$(CC) $(LDFLAGS) -o $@ $(OBJS)
	@echo "Creating symlink $(SYMLINK)..."
	@ln -sf $(NAME) $(SYMLINK)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $@

clean:
	@rm -rf $(OBJDIR)
	@echo "Objects removed."

fclean: clean
	@rm -f $(NAME) $(SYMLINK) test_program
	@echo "Library removed."

re: fclean all

# --- Test Rule ---

# Compiles the test program linking against your shared library
test: $(NAME)
	$(CC) $(CFLAGS) -o test_program test.c -L. -lft_malloc_$(HOSTTYPE) -Wl,-rpath=.
	@echo "\nRun ./run.sh to execute the test."

.PHONY: all clean fclean re test