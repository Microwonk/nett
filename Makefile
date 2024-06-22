compile:
	meson compile -C build

setup:
	python meson-vcpkg-generate.py && meson setup --reconfigure build --native-file meson-vcpkg.txt --wrap-mode nodownload

start:
	./build/nett

clean:
	rm -rf build vcpkg_installed meson-vcpkg.txt

run: compile start
