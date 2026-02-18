SRCS = \
       $(wildcard ./src/*.cpp)

GRN = "\e[1;32m[
END = ]\e[0m"

MSG = @echo $(GRN)

CXX = c++
CXXFLAGS = -g -Wall -Wextra -Werror -std=c++98 -I./include
#CXXFLAGS = -g -std=c++98 -I./include
OBJS = $(SRCS:.cpp=.o)

NAME = ircserv

all : $(NAME)

$(NAME) : $(OBJS)
	$(MSG)making $(NAME)📂$(END)
	$(CXX) -o $(NAME) $(OBJS)
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
