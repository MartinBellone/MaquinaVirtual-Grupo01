# Compilador y flags
CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -Iinclude -Iinclude/mnemonics
LDFLAGS = 

# Carpetas
SRC_DIR    = src
MNEM_DIR   = src/mnemonics
INC_DIR    = include
BUILD_DIR  = build
BIN_DIR    = bin

# Archivos fuente
SRC = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(MNEM_DIR)/*.c)

# Archivos objeto (mismo nombre que .c pero en build/)
OBJ = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(filter $(SRC_DIR)/%,$(SRC))) \
      $(patsubst $(MNEM_DIR)/%.c,$(BUILD_DIR)/mnemonics/%.o,$(filter $(MNEM_DIR)/%,$(SRC)))

# Ejecutable final
TARGET = $(BIN_DIR)/vmx

# Regla por defecto
all: $(TARGET)

# Linkeo final
$(TARGET): $(OBJ) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Compilación de cada .c → .o
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/mnemonics/%.o: $(MNEM_DIR)/%.c | $(BUILD_DIR)/mnemonics
	$(CC) $(CFLAGS) -c $< -o $@

# Crear carpetas si no existen
$(BUILD_DIR):
	if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)

$(BUILD_DIR)/mnemonics:
	if not exist $(BUILD_DIR)\mnemonics mkdir $(BUILD_DIR)\mnemonics

$(BIN_DIR):
	if not exist $(BIN_DIR) mkdir $(BIN_DIR)

# Limpieza
clean:
	if exist $(BUILD_DIR) rmdir /S /Q $(BUILD_DIR)
	if exist $(BIN_DIR) rmdir /S /Q $(BIN_DIR)

# Phony targets (no generan archivos con esos nombres)
.PHONY: all clean
