compile:
	cmake --build build

setup:
	cmake --preset=default

start:
	./build/nett

clean:
	rm -rf build

run: compile start
