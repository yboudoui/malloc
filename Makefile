ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif
LIBNAME = ft_malloc
FILENAME = lib${LIBNAME}
NAME = ${FILENAME}_${HOSTTYPE}.so

TEST_PROGRAM_NAME = test_${FILENAME}
TEST_DIR = test
SRC_DIR = render/src
INC_DIR = render/inc

SRC += bins.c
SRC += utils.c
SRC += free.c
SRC += malloc.c
SRC += realloc.c

SOURCES	=	$(addprefix ${SRC_DIR}/, ${SRC})
OBJECTS	=	$(patsubst $(SRC_DIR)/%.c, $(BUILD)/%.o, $(SRC))

CC = clang
FLAGS_ERROR = -Wall -Werror -Wextra
FLAGS_INCLUDES = -I ${INC_DIR}
FLAGS_DEBUG = -g3 #-fsanitize=address
FLAGS_COMPILE = -fPIC -shared
FLAGS = ${FLAGS_INCLUDES} ${FLAGS_ERROR} ${FLAGS_COMPILE} ${FLAGS_DEBUG}

${NAME}:
	@${CC} ${FLAGS} -o ${NAME} ${SOURCES}
	@ln -sf ${NAME} ${FILENAME}.so

all: ${NAME}

test: re
	@${CC} ${FLAGS_DEBUG} -o ${TEST_PROGRAM_NAME} ${TEST_DIR}/main.c -L. -l${LIBNAME}
	sh run.sh; ./${TEST_PROGRAM_NAME}

clean:
	@rm -f ${NAME}
	@rm -f ${FILENAME}.so

re: clean all

fclean: clean
	@rm -f ${TEST_PROGRAM_NAME}