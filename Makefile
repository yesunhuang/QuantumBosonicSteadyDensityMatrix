INCLUDE_DIR 	:= include
OUT_DIR			:= out
SRC_DIR 		:= src
OBJ_DIR 		:= $(OUT_DIR)/obj
BIN_DIR 		:= $(OUT_DIR)/bin
_SRC			:= main.cpp
SRC				:= $(_SRC:%=$(SRC_DIR)/%)
OBJ				:= $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
EXE				:= $(BIN_DIR)/hello.exe

CXX				:= g++
LDFLAGS			:= 
LDLIBS			:= 
CXXFLAG 		:= -I$(INCLUDE_DIR) -Wall 

LINTER			:= cpplint
LINTER_FLAG		:= --quiet --verbose=2 --linelength=100 --recursive

CHECKER			:= cppcheck
CHECKER_FLAG	:= --language=c++ --quiet --std=c11 --enable=all -I $(INCLUDE_DIR) --suppress=missingIncludeSystem

.PHONY: all clean run lint check

all: $(EXE)

$(EXE): $(OBJ) | $(BIN_DIR)
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAG) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

run: $(EXE)
	$(EXE)

clean:
	rm -rf $(OUT_DIR)

lint:
	$(LINTER) $(LINTER_FLAG) $(SRC_DIR) $(INCLUDE_DIR)

check:
	$(CHECKER) $(CHECKER_FLAG) $(SRC_DIR)
