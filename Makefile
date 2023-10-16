NAME =	webserv

SRCS =	configHandler/configHandler.cpp \
		configHandler/get_servers_from_config.cpp \
		utils/trim.cpp \
		utils/split.cpp \
		utils/join.cpp \
		utils/verify_values.cpp \
		utils/normstring.cpp \
		utils/debug.cpp \
		utils/conversion.cpp \
		utils/atoi_base.cpp \
		utils/matching_length.cpp \
		utils/handleFile.cpp \
		utils/get_time.cpp \
		utils/toString.cpp \
		utils/url_encoding.cpp \
		utils/case.cpp \
		utils/is_valid_ip.cpp \
		server/server.cpp \
		server/init_server.cpp \
		server/location.cpp \
		socketManaging/Loop/loop.cpp \
		socketManaging/Loop/loopUtils.cpp \
		socketManaging/fdHandler/fdHandler.cpp \
		requestHandler/httpRequest.cpp \
		requestHandler/httpRequestCore.cpp \
		http/httpResponse.cpp \
		http/errorPages.cpp \
		http/fileDetails.cpp \
		http/handleFile.cpp \
		server/base_server_config.cpp \
		CGI/cgi.cpp

SRCSDIR = sources

SRCS := $(addprefix $(SRCSDIR)/, $(SRCS))

DEFAULTSRCS := $(SRCS) $(SRCSDIR)/main.cpp

TESTSRCS := $(SRCS) tests/main.cpp \
			tests/split.test.cpp

DEPS =	include/configHandler.hpp \
		include/utils.hpp \
		include/loop.hpp \
		include/test.hpp \
		include/server.hpp \
		include/location.hpp \
		include/Pair.hpp \
		include/fdHandler.hpp \
		include/httpRequest.hpp \
		include/redirectRules.hpp \
		include/cgi.hpp

OBJSRCS = $(DEFAULTSRCS:.cpp=.o)
TESTOBJ = $(TESTSRCS:.cpp=.o)

CC = c++

CFLAGS = -Wall -Wextra -Werror -std=c++98 -g3

.cpp.o :
	$(CC) $(CFLAGS) -c $< -o ${<:.cpp=.o}

$(NAME): $(OBJSRCS) $(DEPS)
	@echo "\033[33m[Compiliation in progress...]\033"
	$(CC) $(CFLAGS) $(OBJSRCS) -o $(NAME)
	@echo "\033[32m[Success !]"

run: $(NAME)
	@echo "\033[0m"
	./$(NAME)

test: $(TESTOBJ) $(DEPS)
	@echo "\033[33m[Compiliation in progress...]\033"
	$(CC) $(CFLAGS) $(TESTSRCS) -o $(NAME)_test
	@echo "\033[32m[Success !]"

all: $(NAME)

clean:
	rm -f $(OBJSRCS)

fclean: clean
	rm -f $(NAME)
	rm -f $(NAME)_test

re: fclean all

.PHONY: all re clean fclean test
