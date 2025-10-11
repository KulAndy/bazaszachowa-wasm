EMCC        := em++
STD         := -std=c++17
INCLUDES    := -I chess-library/include
EMFLAGS     := --bind -s MODULARIZE=1 -s EXPORT_ES6=1 -s ENVIRONMENT=web -s ALLOW_MEMORY_GROWTH=1

SRC_DIR     := src
BUILD_DIR   := build
EXAMPLE_DIR := example

TARGETS     := $(BUILD_DIR)/chess_processor.js \
               $(BUILD_DIR)/uci2pgn.js \
               $(BUILD_DIR)/game_stats.js \
               $(BUILD_DIR)/stats.js

WASM_FILES  := $(TARGETS:.js=.wasm)

all: $(TARGETS)

$(BUILD_DIR):
	@mkdir -p $@

$(BUILD_DIR)/chess_processor.js: $(SRC_DIR)/game2fens/main.cpp | $(BUILD_DIR)
	@echo "Compiling $< → $@"
	$(EMCC) $(STD) $(INCLUDES) $(EMFLAGS) $< -o $@

$(BUILD_DIR)/uci2pgn.js: $(SRC_DIR)/js-chess-uci2san/main.cpp | $(BUILD_DIR)
	@echo "Compiling $< → $@"
	$(EMCC) $(STD) $(INCLUDES) $(EMFLAGS) $< -o $@

$(BUILD_DIR)/game_stats.js: $(SRC_DIR)/game_stats/main.cpp | $(BUILD_DIR)
	@echo "Compiling $< → $@"
	$(EMCC) $(STD) $(INCLUDES) $(EMFLAGS) $< -o $@

$(BUILD_DIR)/stats.js: $(SRC_DIR)/stats/main.cpp | $(BUILD_DIR)
	@echo "Compiling $< → $@"
	$(EMCC) $(STD) $(INCLUDES) $(EMFLAGS) $< -o $@

example: all
	@echo "Building example..."
	cp $(WASM_FILES) $(TARGETS) $(EXAMPLE_DIR)/build
	cp $(BUILD_DIR)/*.js $(EXAMPLE_DIR)/src/
	cd $(EXAMPLE_DIR) && \
	npm install && \
	npx tsc

clean:
	@echo "Cleaning build directory..."
	rm -rf $(BUILD_DIR)

.PHONY: all clean example
