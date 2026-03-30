NAME		= ircserv

CXX			= c++
CXXFLAGS	= -Wall -Wextra -Werror -std=c++98

INCLUDES	= -I includes

SRCS_DIR	= srcs
OBJS_DIR	= objs

SRCS		= $(SRCS_DIR)/main.cpp \
			  $(SRCS_DIR)/Server.cpp \
			  $(SRCS_DIR)/Client.cpp \
			  $(SRCS_DIR)/Channel.cpp \
			  $(SRCS_DIR)/Message.cpp

OBJS		= $(SRCS:$(SRCS_DIR)/%.cpp=$(OBJS_DIR)/%.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.cpp | $(OBJS_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(OBJS_DIR):
	mkdir -p $(OBJS_DIR)

clean:
	rm -rf $(OBJS_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
