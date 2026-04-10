NAME		= ircserv
BOT			= bot
BOT_SRCS	= $(SRCS_DIR)/Bot.cpp
BOT_OBJS	= $(BOT_SRCS:$(SRCS_DIR)/%.cpp=$(OBJS_DIR)/%.o)

CXX			= c++
CXXFLAGS	= -Wall -Wextra -Werror -std=c++98

INCLUDES	= -I includes

SRCS_DIR	= srcs
OBJS_DIR	= objs

SRCS		= $(SRCS_DIR)/main.cpp \
			  $(SRCS_DIR)/ServerCommands.cpp\
			  $(SRCS_DIR)/ServerNetwork.cpp\
			  $(SRCS_DIR)/Client.cpp \
			  $(SRCS_DIR)/Channel.cpp \
			  $(SRCS_DIR)/Message.cpp

OBJS		= $(SRCS:$(SRCS_DIR)/%.cpp=$(OBJS_DIR)/%.o)

all: $(NAME) $(BOT)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

$(BOT): $(BOT_OBJS)
	$(CXX) $(CXXFLAGS) $(BOT_OBJS) -o $(BOT)

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.cpp | $(OBJS_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(OBJS_DIR):
	mkdir -p $(OBJS_DIR)

clean:
	rm -rf $(OBJS_DIR)

fclean: clean
	rm -f $(NAME) $(BOT)

re: fclean all

.PHONY: all clean fclean re
