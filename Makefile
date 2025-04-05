CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -pedantic -g
CFLAGS = -DUNITY_OUTPUT_COLOR=1
INCLUDES = -I./include -I./third_party -I./third_party/Unity/src -I./third_party/Unity/extras/fixture/src -I./third_party/Unity/extras/memory/src
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
TEST_DIR = test
TUI_DIR = tui
DEPS_DIR = third_party

# Color definitions
GREEN = \033[0;32m
YELLOW = \033[0;33m
CYAN = \033[0;36m
RESET = \033[0m

# Source files
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
TESTS = $(wildcard $(TEST_DIR)/*.cpp)
TESTOBJS = $(TESTS:$(TEST_DIR)/%.cpp=$(OBJ_DIR)/%.o)
HEADERS = $(wildcard include/*.hpp)
EXECUTABLE = $(BIN_DIR)/chess_game
TEST = $(BIN_DIR)/chess_test
ALIB = $(BIN_DIR)/libchess.a
ULIB = $(BIN_DIR)/libunity.a

VPATH := $(TEST_DIR):$(SRC_DIR):$(TUI_DIR)

all: deps $(EXECUTABLE) $(TEST)
	@printf "$(GREEN)Building executable complete! Run ./$(EXECUTABLE) to start the project.$(RESET)\n"
	@printf "$(GREEN)Build test suite complete! Run ./$(TEST) -v to start the test suite.$(RESET)\n"

deps:
	@printf "$(YELLOW)Checking dependencies...$(RESET)\n"
	@if [ ! -f "$(DEPS_DIR)/nlohmann/json.hpp" ]; then \
		printf "$(YELLOW)Downloading JSON library...$(RESET)\n"; \
		mkdir -p $(DEPS_DIR)/nlohmann; \
		curl -L https://github.com/nlohmann/json/releases/download/v3.11.2/json.hpp \
			-o $(DEPS_DIR)/nlohmann/json.hpp; \
	fi
	@if [ ! -e "$(DEPS_DIR)/Unity" ]; then \
		printf "$(YELLOW)Downloading Unity Testing Framework...$(RESET)\n"; \
		mkdir -p $(DEPS_DIR)/Unity; \
		curl -L https://github.com/ThrowTheSwitch/Unity/archive/refs/tags/v2.6.0.tar.gz \
			-o $(DEPS_DIR)/Unity/v2.6.0.tar.gz; \
		tar xzf $(DEPS_DIR)/Unity/v2.6.0.tar.gz -C $(DEPS_DIR)/Unity/ --strip-components=1; \
	fi

$(EXECUTABLE): $(OBJ_DIR)/main.o $(ALIB)
	@printf "$(YELLOW)Linking chess_game...$(RESET)\n"
	@$(CXX) $^ -o $@
	@printf "$(GREEN)Linking complete!$(RESET)\n"

$(TEST): $(TESTOBJS) $(ALIB) $(ULIB)
	@printf "$(YELLOW)Linking chess_test...$(RESET)\n"
	@$(CXX) $^ -o $@
	@printf "$(GREEN)Linking complete!$(RESET)\n"

$(ALIB): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	@printf "$(YELLOW)Linking libchess.a...$(RESET)\n"
	@$(AR) rcs $@ $^
	@printf "$(GREEN)Linking complete!$(RESET)\n"

$(OBJ_DIR)/%.o: %.cpp $(HEADERS)
	@mkdir -p $(OBJ_DIR)
	@printf "$(CYAN)Compiling $<...$(RESET)\n"
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(ULIB):
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(BIN_DIR)
	@if [ ! -e "$(DEPS_DIR)/Unity" ]; then \
	printf "$(CYAN)Unity Testing Framework not found. Did you run make deps?$(RESET)\n"; \
	exit 1; \
	fi

	@printf "$(GREEN)Compiling Unity...$(RESET)\n"
	@$(CC) $(CFLAGS) $(INCLUDES) -c \
	$(DEPS_DIR)/Unity/src/unity.c \
	$(DEPS_DIR)/Unity/extras/fixture/src/unity_fixture.c
	@$(AR) rcs $@ unity.o unity_fixture.o
	@rm -f unity.o unity_fixture.o
	@printf "$(GREEN)Unity compiled!$(RESET)\n"

clean:
	@printf "$(YELLOW)Cleaning up...$(RESET)\n"
	@rm -rf $(OBJ_DIR) $(BIN_DIR)
	@printf "$(GREEN)Cleanup complete!$(RESET)\n"

distclean: clean
	@printf "$(YELLOW)Removing dependencies...$(RESET)\n"
	@rm -rf $(DEPS_DIR)
	@printf "$(GREEN)Dependencies removed!$(RESET)\n"

run: $(EXECUTABLE)
	@printf "$(GREEN)Running the project with chess_pieces.json...$(RESET)\n"
	@./$(EXECUTABLE) data/chess_pieces.json

run_fantasy: $(EXECUTABLE)
	@printf "$(GREEN)Running the project with fantasy_chess.json...$(RESET)\n"
	@./$(EXECUTABLE) data/fantasy_chess.json

test: $(ULIB) $(TEST)
	@printf "$(YELLOW)Running the test suite...$(RESET)\n"
	@./$(TEST) -v; \
	if [ $$? -eq 0 ]; then \
		printf "$(GREEN)All tests passed!$(RESET)\n"; \
	else \
		printf "$(CYAN)Some tests failed.$(RESET)\n"; \
	fi

.PHONY: all clean distclean run deps test