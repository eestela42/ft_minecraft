################################################################################
#                               Filename output                                #
################################################################################

NAME		=	ft_vox

################################################################################
#                               Sources filenames                              #
################################################################################

CPP_FILES = $(shell find $(SRCS_DIR) -name '*.cpp')
C_FILES = $(shell find $(SRCS_DIR) -name '*.c')

################################################################################
#                         Sources and objects directories                      #
################################################################################

SRCS_DIR = srcs
OBJS_DIR = objects-dependances
DEPS_DIR = $(OBJS_DIR)

################################################################################
#                              Commands and arguments                          #
################################################################################

CC			=	clang++ 
CFLAGS = -std=c++17 -g -Iincludes -MMD -MP
OPENGL = -lglfw3 -lGL -lX11 -llmdb
RM			=	rm -rf

################################################################################
#                                 Defining colors                              #
################################################################################

_RED		=	\033[31m
_GREEN		=	\033[32m
_YELLOW		=	\033[33m
_CYAN		=	\033[96m
_NC			=	\033[0m

################################################################################
#                                    Objects                                    #
################################################################################

CPP_OBJS = $(patsubst $(SRCS_DIR)/%.cpp,$(OBJS_DIR)/%.o,$(CPP_FILES))
C_OBJS = $(patsubst $(SRCS_DIR)/%.c,$(OBJS_DIR)/%.o,$(C_FILES))

$(shell mkdir -p $(sort $(dir $(CPP_OBJS))) $(sort $(dir $(C_OBJS))))

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.c
	@ echo "\t$(_YELLOW) compiling... $*.c$(_NC)"
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.cpp
	@ echo "\t$(_YELLOW) compiling... $*.cpp$(_NC)"
	@$(CC) $(CFLAGS) -c $< -o $@

################################################################################
#                                  Dependances                                 #
################################################################################

CPP_DEPS = $(patsubst $(OBJS_DIR)/%.o,$(DEPS_DIR)/%.d,$(CPP_OBJS))
C_DEPS = $(patsubst $(OBJS_DIR)/%.o,$(DEPS_DIR)/%.d,$(C_OBJS))
DEP_FILES = $(CPP_DEPS) $(C_DEPS)

$(shell mkdir -p $(sort $(dir $(DEP_FILES))))

$(DEPS_DIR)/%.d: $(SRCS_DIR)/%.cpp | $(DEPS_DIR)
	@ echo "\t$(_YELLOW) compiling... $*.d$(_NC)"
	@$(CC) $(CFLAGS) -MM $< -MT $(@:.d=.o) -MF $@

$(DEPS_DIR)/%.d: $(SRCS_DIR)/%.c | $(DEPS_DIR)
	@ echo "\t$(_YELLOW) compiling... $*.d$(_NC)"
	@$(CC) $(CFLAGS) -MM $< -MT $(@:.d=.o) -MF $@

################################################################################
#                                   Command                                    #
################################################################################

all: init $(NAME)

init:
	@ if test -f $(NAME);\
		then echo "$(_CYAN)[program already created]$(_NC)";\
		else \
		echo "$(_YELLOW)[Initialize program]$(_NC)";\
		$(shell mkdir -p $(sort $(dir $(CPP_OBJS))) $(sort $(dir $(C_OBJS)))) \
	fi

$(NAME): $(CPP_OBJS) $(C_OBJS)
	@ echo "\t$(_YELLOW)[Creating program]$(_NC)"
	@$(CC) $(CFLAGS) $(CPP_OBJS) $(C_OBJS) $(OPENGL) -Llibs -o $(NAME) -ldl -lpthread
	@ echo "$(_GREEN)[program created & ready]$(_NC)"

clean:
	echo "$(_RED)[cleaning up .out & objects files]"
	@$(RM) $(OBJS_DIR)
	@$(RM) $(DEPS_DIR)

fclean: clean
	@ echo "$(_RED)[cleaning up .out, objects & library files]$(_NC)"
	@$(RM) $(NAME)

re: fclean all

.SILENT:
		all

.PHONY: all clean fclean re

-include $(DEP_FILES)