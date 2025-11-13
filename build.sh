mkdir -p build

cp /usr/x86_64-w64-mingw32/bin/SDL2.dll build

x86_64-w64-mingw32-g++ -std=c++17 -o build/main.exe src/*.cpp src/*/*.cpp -lmingw32 -lSDL2main -lSDL2 -Wl,-Bstatic -lstdc++ -Wl,-Bdynamic -lSDL2 -static-libgcc -static-libstdc++ -mwindows && \
echo "compiled windows executable"

g++ -std=c++17 -o build/main.out src/*.cpp src/*/*.cpp -lSDL2 && \
echo "compiled unix executable"
