LLVM_ROOT_PATH := llvm
LLVM_SRC_PATH := $(LLVM_ROOT_PATH)/llvm
LLVM_BUILD_PATH := $(LLVM_ROOT_PATH)/build
LLVM_BIN_PATH := $(LLVM_BUILD_PATH)/bin

$(info -----------------------------------------------)
$(info Using LLVM_SRC_PATH = $(LLVM_SRC_PATH))
$(info Using LLVM_BUILD_PATH = $(LLVM_BUILD_PATH))
$(info -----------------------------------------------)

CXX := g++
CXXFLAGS := -fno-rtti -O0 -g -dM 
LLVM_CXXFLAGS := `$(LLVM_BIN_PATH)/llvm-config --cxxflags`
LLVM_LDFLAGS := `$(LLVM_BIN_PATH)/llvm-config --ldflags --libs --system-libs`

CLANG_INCLUDES := \
	-I$(LLVM_SRC_PATH)/tools/clang/include \
	-I$(LLVM_BUILD_PATH)/tools/clang/include \
	-I$(LLVM_SRC_PATH)/include \
	-I$(LLVM_BUILD_PATH)/include

# List of Clang libraries to link. The proper -L will be provided by the
# call to llvm-config
# Note that I'm using -Wl,--{start|end}-group around the Clang libs; this is
# because there are circular dependencies that make the correct order difficult
# to specify and maintain. The linker group options make the linking somewhat
# slower, but IMHO they're still perfectly fine for tools that link with Clang.

CLANG_LIBS := \
	-Wl,--start-group \
	-lclangAST \
	-lclangAnalysis \
	-lclangBasic \
	-lclangDriver \
	-lclangEdit \
	-lclangFrontend \
	-lclangFrontendTool \
	-lclangLex \
	-lclangParse \
	-lclangSema \
	-lclangEdit \
	-lclangASTMatchers \
	-lclangRewrite \
	-lclangRewriteFrontend \
	-lclangStaticAnalyzerFrontend \
	-lclangStaticAnalyzerCheckers \
	-lclangStaticAnalyzerCore \
	-lclangSerialization \
	-lclangTooling \
	-Wl,--end-group 

# Internal paths in this project: where to find sources, and where to put
# build artifacts.
SRC_DIR := src
LIB_DIR := inc

MAINOBJ=$(SRC_DIR)/Main.o

SOURCES=$(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(filter-out $(MAINOBJ), $(SOURCES:%.cpp=%.o))

.PHONY: all
all: lex \
	basic \
	basic_fin\
	rewriter\
	p1\
	p2

.PHONY: make_build
lex : $(SRC_DIR)/lex_example.cpp
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) $(CLANG_INCLUDES) $^ \
	$(CLANG_LIBS) $(LLVM_LDFLAGS) -o $@

basic : $(SRC_DIR)/basic_llvm.cpp
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) $(CLANG_INCLUDES) $^ \
	$(CLANG_LIBS) $(LLVM_LDFLAGS) -o $@

basic_fin : $(SRC_DIR)/basic_llvm_fin.cpp
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) $(CLANG_INCLUDES) $^ \
	$(CLANG_LIBS) $(LLVM_LDFLAGS) -o $@

rewriter : $(SRC_DIR)/rewriter_llvm.cpp
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) $(CLANG_INCLUDES) $^ \
	$(CLANG_LIBS) $(LLVM_LDFLAGS) -o $@

p1 : $(SRC_DIR)/practice_llvm1.cpp
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) $(CLANG_INCLUDES) $^ \
	$(CLANG_LIBS) $(LLVM_LDFLAGS) -o $@


p2 : $(SRC_DIR)/practice_llvm2.cpp
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) $(CLANG_INCLUDES) $^ \
	$(CLANG_LIBS) $(LLVM_LDFLAGS) -o $@

.PHONY: clean
clean:
	rm -f $(SRC_DIR)/*.o
	rm -f lex basic basic_fin rewriter p1 p2
