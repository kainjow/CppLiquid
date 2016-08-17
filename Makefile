.PHONY: xcode

xcode:
	mkdir -p build
	cd build && cmake -G "Xcode" .. && open *.xcodeproj
