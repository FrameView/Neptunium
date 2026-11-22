SRC = chunk.c debug.c main.c memory.c value.c vm.c
OBJ = $(SRC:%.c=build/%.o)
TARGET = build/neptunium

# Step 2: link the objects
all: $(OBJ)
	gcc $(OBJ) -o $(TARGET)
	@echo "Build completed: $(TARGET)"

# Step 1: compile each .c into a .o
build/%.o: %.c
	gcc -c $< -o $@

# Init
init:
	mkdir -p build

# Run
run:
	$(TARGET)

# Cleanup
clean:
	rm -f build/*
	@echo "Cleaned build files"

.PHONY: all init run clean