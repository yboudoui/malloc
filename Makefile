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

SRC += utils/bins.c
SRC += utils/block.c
SRC += utils/page.c
SRC += utils/utils.c
SRC += malloc.c
SRC += show.c

SOURCES	=	$(addprefix ${SRC_DIR}/, ${SRC})
OBJECTS	=	$(patsubst $(SRC_DIR)/%.c, $(BUILD)/%.o, $(SRC))

CC = clang
FLAGS_ERROR = -Wall -Werror -Wextra
FLAGS_INCLUDES = -I ${INC_DIR}
FLAGS_DEBUG = -g3
FLAGS_COMPILE = -fPIC -shared
FLAGS = ${FLAGS_INCLUDES} ${FLAGS_ERROR} ${FLAGS_COMPILE} ${FLAGS_DEBUG}

${NAME}:
	@${CC} ${FLAGS} -o ${NAME} ${SOURCES}
	@ln -sf ${NAME} ${FILENAME}.so

all: ${NAME}

test: re
	${CC} ${FLAGS_DEBUG}  -L. -l${LIBNAME} ${TEST_DIR}/main.c -o ${TEST_PROGRAM_NAME}
	sh run.sh; export LD_LIBRARY_PATH=.; ./${TEST_PROGRAM_NAME}

clean:
	@rm -f ${NAME}
	@rm -f ${FILENAME}.so

re: clean all

fclean: clean
	@rm -f ${TEST_PROGRAM_NAME}