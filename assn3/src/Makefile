CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -MMD -MP
INCLUDES := -I. -I../../include
LIBS := -lGL -lGLEW -lglut
BIN := main

.SILENT:

SRC_DIRS := \
	app \
	core/base \
	game/entities \
	game/weapons \
	game/attachments \
	game/ui

SRCS := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))
OBJS := $(patsubst %.cpp,build/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

.PHONY: all clean run

all: $(BIN)

$(BIN): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBS)
	@echo "[All] $(BIN) built"

build/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

run: $(BIN)
	./$(BIN)
	@echo "[RUN] $(BIN) finished"

clean:
	rm -rf build $(BIN)
	@echo "[CLEAN] build artifacts removed"

-include $(DEPS)
