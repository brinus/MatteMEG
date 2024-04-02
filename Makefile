# Definizione dei comandi e delle opzioni
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11
ROOTCONFIG = $(shell root-config --glibs --cflags --libs)
INC_DIR = include
SRC_DIR = src
LIB_DIR = lib

# Definizione dei file sorgente e oggetto
SRC_FILES = $(wildcard $(SRC_DIR)/*.C)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.C,$(LIB_DIR)/%.o,$(SRC_FILES))

# Target per la compilazione dell'oggetto
all: $(LIB_DIR)/libMMUtils.so

# Regole per la compilazione degli oggetti
$(LIB_DIR)/%.o: $(SRC_DIR)/%.C
	$(CXX) $(CXXFLAGS) $(ROOTCONFIG) -I$(INC_DIR) -fPIC -c $< -o $@

# Target per la creazione della libreria condivisa
$(LIB_DIR)/libMMUtils.so: $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) $(ROOTCONFIG) -shared -o $@ $^

# Pulizia dei file intermedi e della libreria
clean:
	rm -f $(LIB_DIR)/*.o $(LIB_DIR)/libMMUtils.so

# Phony target per evitare conflitti con file con lo stesso nome
.PHONY: all clean

