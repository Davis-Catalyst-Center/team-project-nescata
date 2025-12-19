# ==========================================
# Settings
# ==========================================

# Compilers
CXX       = g++
WIN_CXX   = x86_64-w64-mingw32-g++
WIN_RES   = x86_64-w64-mingw32-windres

# Directories
SRC_DIR   = src
RES_DIR   = resources
BUILD_DIR = build
INC       = -Iinclude

# Source Files
SRCS      = $(wildcard $(SRC_DIR)/*.cpp)

# Output Files
OUT_LINUX = $(BUILD_DIR)/nescata
OUT_WIN   = $(BUILD_DIR)/nescata.exe

# Icon Files
SVG_ICON  = $(RES_DIR)/logo.svg
ICO_ICON  = $(RES_DIR)/logo.ico
RC_FILE   = $(SRC_DIR)/resources.rc
RES_OBJ   = $(BUILD_DIR)/resources.o

# General Flags
CXXFLAGS  = -std=c++17 -g

# ------------------------------------------
# Windows Specific Flags
# ------------------------------------------
WIN_SDL_LIB = /usr/x86_64-w64-mingw32/lib/libSDL2.a

WIN_LDFLAGS = \
			-static -static-libgcc -static-libstdc++ \
			-L/usr/x86_64-w64-mingw32/lib \
			-lmingw32 -lSDL2main $(WIN_SDL_LIB) \
			-mwindows -Wl,--dynamicbase -Wl,--nxcompat -Wl,--high-entropy-va -lm \
			-ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32 \
			-loleaut32 -lshell32 -lsetupapi -lversion -luuid

# ------------------------------------------
# Linux Specific Flags (Configured for Mixed Static/Dynamic Linking)
# ------------------------------------------
# 1. Get the exact path to the static libSDL2.a file
SDL_STATIC_LIB = $(shell pkg-config --variable=libdir sdl2)/libSDL2.a

# 2. Get the required system dependencies (like ALSA, X11, pthread)
#    We use sed to remove the -lSDL2 flag that pkg-config adds, as we link the .a file directly
SDL_SYS_LIBS = $(shell pkg-config --static --libs sdl2 | sed 's/-lSDL2 //')

# ==========================================
# Rules
# ==========================================

.PHONY: all clean windows linux run debug

all: windows linux

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# ------------------------------------------
# Windows Build
# ------------------------------------------

# 1. Convert SVG to ICO if needed
$(ICO_ICON): $(SVG_ICON)
	@echo "Converting SVG to ICO..."
	convert -background none $(SVG_ICON) -define icon:auto-resize=256,128,64,48,32,16 $(ICO_ICON)

# 2. Compile Resource File (.rc -> .o)
$(RES_OBJ): $(RC_FILE) $(ICO_ICON) | $(BUILD_DIR)
	@echo "Compiling Resources..."
	$(WIN_RES) $(RC_FILE) -o $(RES_OBJ)

# 3. Link everything together
windows: $(BUILD_DIR) $(SRCS) $(RES_OBJ)
	@echo "Building for Windows..."
	cp /usr/x86_64-w64-mingw32/bin/SDL2.dll $(BUILD_DIR)
	$(WIN_CXX) $(CXXFLAGS) $(INC) -o $(OUT_WIN) $(SRCS) $(RES_OBJ) $(WIN_LDFLAGS)
	@echo "Compiled Windows executable: $(OUT_WIN)"

# ------------------------------------------
# Linux Build
# ------------------------------------------
linux: $(BUILD_DIR) $(SRCS)
	@echo "Building for Linux (Mixed Static/Dynamic - Standalone)..."
	# Key change: linking SDL2 statically, but system libs dynamically
	$(CXX) $(CXXFLAGS) $(INC) -o $(OUT_LINUX) $(SRCS) \
		$(SDL_STATIC_LIB) \
		-static-libgcc -static-libstdc++ \
		$(SDL_SYS_LIBS)
	@echo "Compiled Unix executable: $(OUT_LINUX)"

# ------------------------------------------
# Helper Commands (These remain dynamic for faster local compilation/debugging)
# ------------------------------------------
run: $(BUILD_DIR) $(SRCS)
	@echo "Compiling and Running..."
	$(CXX) $(CXXFLAGS) $(INC) -o $(OUT_LINUX) $(SRCS) $(shell pkg-config --libs sdl2)
	./$(OUT_LINUX) $(ARGS)

debug: $(BUILD_DIR) $(SRCS)
	@echo "Compiling for Debug..."
	$(CXX) $(CXXFLAGS) -g $(INC) -o $(OUT_LINUX) $(SRCS) $(shell pkg-config --libs sdl2)
	gdb --args ./$(OUT_LINUX) $(ARGS)

clean:
	rm -rf $(BUILD_DIR)
	# Optional: Remove generated ICO to force regeneration
	# rm -f $(ICO_ICON)
