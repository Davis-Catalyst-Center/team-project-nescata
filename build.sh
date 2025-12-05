mkdir -p build

cp /usr/x86_64-w64-mingw32/bin/SDL2.dll build

x86_64-w64-mingw32-g++ -g -std=c++17 -Iinclude -o build/main.exe src/*.cpp \
	-lmingw32 -lSDL2main -lSDL2 -Wl,-Bstatic -lstdc++ -Wl,-Bdynamic \
	-lSDL2 -static -static-libgcc -static-libstdc++ -mwindows -lwinmm \
	-lole32 -loleaut32 -limm32 -lsetupapi -lversion -luuid -lgdi32 -luser32 && \
echo "compiled windows executable"

g++ -std=c++17 -Iinclude -o build/main.out src/*.cpp -lSDL2 && \
echo "compiled unix executable"
