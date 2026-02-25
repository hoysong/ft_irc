SRCS = \
       ./srcs/main.cpp \
       $(wildcard ./srcs/client/*.cpp) \
       $(wildcard ./srcs/clientManager/*.cpp) \
       $(wildcard ./srcs/epoll/*.cpp) \
       $(wildcard ./srcs/listen/*.cpp) \
       $(wildcard ./srcs/ircserver/*.cpp) \
       $(wildcard ./srcs/stateMachine/*.cpp) \
       $(wildcard ./srcs/msgBuilder/*.cpp) \
       $(wildcard ./srcs/channel/*.cpp)

GRN = "\e[1;32m[
END = ]\e[0m"

MSG = @echo $(GRN)

CXX = c++
#CXXFLAGS = -g -Wall -Wextra -Werror -std=c++98 -I./includes -fsanitize=address
CXXFLAGS = -g -std=c++98 -I./includes -fsanitize=address
#CXXFLAGS = -g -std=c++98 -I./includes
OBJS = $(SRCS:.cpp=.o)

NAME = ircserv

all : $(NAME)

$(NAME) : $(OBJS)
	$(MSG)making $(NAME)📂$(END)
	$(CXX) -fsanitize=address -o $(NAME) $(OBJS)
	$(MSG)make done✔$(END)

clean :
	@clear
	$(MSG)delete *.o🧹$(END)
	rm -f $(OBJS)

fclean : clean
	$(MSG)delete $(NAME)🧹$(END)
	rm -f $(NAME)

re : fclean
	$(MSG)make re..🔄$(END)
	make $(NAME)
