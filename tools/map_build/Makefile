map_build: hash_map.o map_build.o main.o
	@ g++ hash_map.o map_build.o main.o -o map_build

hash_map.o: hash_map.cpp
	@ g++ -O2 -c hash_map.cpp -o hash_map.o

map_build.o: map_build.cpp
	@ g++ -O2 -c map_build.cpp -o map_build.o

main.o: main.cpp
	@ g++ -O2 -c main.cpp -o main.o

clean:
	@ rm -rf hash_map.o map_build.o main.o
