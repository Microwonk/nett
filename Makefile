compile: 
	cmake --build build

setup: 
	cmake --preset=default

s: 
	./build/server/server

c: 
	./build/client/client

clean: 
	rm -rf build

runserver: compile s

runclient: compile c
