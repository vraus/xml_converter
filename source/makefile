CC = g++
CFLAGS = -g -Wall

# Cibles pour debug et release
TARGET = main
RELEASE_TARGET = bin/main
# RELEASE_TARGET = bin/main.exe

INCLUDE_DIR = public
PRIVATE_SRC_DIR = private
BUILD_DIR = build
ROOT_SRC = main.cpp

# Recherche automatique des fichiers sources
SRCS = $(ROOT_SRC) $(wildcard $(PRIVATE_SRC_DIR)/*.cpp)
INCLUDES = -I$(INCLUDE_DIR)

# Générer les fichiers objets dans le dossier build
OBJS = $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(notdir $(SRCS)))

# La règle par défaut pour compiler en mode debug
all: $(BUILD_DIR) $(TARGET)

# Création de l'exécutable debug en liant les fichiers objets
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(OBJS)

# Compilation des fichiers .cpp en .o, placés dans le dossier build
$(BUILD_DIR)/%.o: $(PRIVATE_SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/main.o: main.cpp | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Création du dossier build si nécessaire
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Nettoyage des fichiers objets et de l'exécutable
clean:
	$(RM) $(BUILD_DIR)/*.o $(TARGET) $(RELEASE_TARGET)

# Création du dossier bin si nécessaire
bin:
	mkdir -p bin

# Compilation en mode release
release: CFLAGS = -O3 -Wall
release: $(OBJS) | bin
	$(CC) $(CFLAGS) $(INCLUDES) -o $(RELEASE_TARGET) $(OBJS)
