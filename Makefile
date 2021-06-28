## DIRECTORIES ##
BUILD_DIR := build
BIN_DIR   := bin
SRC_DIR   := src
INC_DIR   := headers
TARGET    := target
MAIN 	  := main.c

## OPTIONS ##
CC        := gcc
OPTS      := 
INC_HDR   := -Iheaders

## LINKING ## 
LD_FLAGS :=
LD_LIBS  := 

## SOURCES ## 
SOURCES  := $(shell find $(SRC_DIR) -type f -name \*.c -not -name $(MAIN))
HEADERS  := $(shell find $(INC_DIR) -type f -name \*.h -not -name $(MAIN))

## OBJECTS ## 
OBJS     := $(strip $(patsubst %.c, %.o, $(notdir $(SOURCES))))

## TARGETS ##
.PHONY : clean setup all printenv

## Build the binary ##
all: setup $(BIN_DIR)/$(TARGET)
	@echo 
	@echo
	@echo "Linking [ $@ ] complete."

## Clean out the build & bin directories ##
clean: 
	@echo
	@echo
	@echo "Cleaning $(BUILD_DIR), $(BIN_DIR).."
	rm -rf $(BUILD_DIR)
	rm -rf $(BIN_DIR)

## Setup build & bin ## 
setup:
	@echo 
	@echo 
	@echo "Setting up output directories..."
	mkdir -p $(BUILD_DIR) $(BIN_DIR)


printenv:
	@echo "OBJECTS       : $(OBJS)"
	@echo "SOURCES       : $(SOURCES)"
	@echo "HEADERS       : $(HEADERS)"
	@echo "BINARY TARGET : $(BIN_DIR)/$(TARGET)"

$(BIN_DIR)/$(TARGET): $(BUILD_DIR)/$(OBJS)
	@echo 
	@echo 
	@echo "Building output binary [ $@ ]..."
	$(CC) $(LD_FLAGS) $(INC_HDR) $(SRC_DIR)/$(MAIN) -o $@ $< $(LD_LIBS)
	
$(BUILD_DIR)/$(OBJS): $(SOURCES) 
	@echo
	@echo
	@echo "Compiling [ $@ ]..."
	$(CC) $(INC_HDR) -o $@ -c $<
