ARGS = -Wall -Wextra -Werror -lstdc++

run: build
	./bin/main

build: clean
	mkdir bin/
	g++ $(ARGS) -o ./bin/main -Iinclude/ main.cpp src/*

clean:
	rm -rf ./bin
