CXX := c++
CXXFLAGS := -Wall -Wextra -Werror -std=c++17 -Iinclude
NAME := wordle

SRCS := \
	src/main.cpp \
	src/Dictionary.cpp \
	src/Evaluator.cpp \
	src/Game.cpp \
	src/TerminalView.cpp

OBJS := $(SRCS:.cpp=.o)

all: $(NAME)

web:
	npm install
	npm run build
	@-pkill -f "http.server 8080"
	@echo "\n\033[1;32mhttp://localhost:8080/web/\033[0m\n"
	python3 -m http.server 8080

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)
	rm -f dist/main.js.map dist/main.d.ts dist/main.d.ts.map

fclean: clean
	rm -f $(NAME)
	rm -rf dist
	rm -rf node_modules

re: fclean all

.PHONY: all clean fclean re web
