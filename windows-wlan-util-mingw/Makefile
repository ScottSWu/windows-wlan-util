CC=g++
CFLAGS=-std=c++11
LIBS=-lwlanapi -lole32
STATIC_LIBS=-static-libgcc -static-libstdc++

windows-wlan-util.exe: windows-wlan-util.cpp
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS) $(STATIC_LIBS)

clean:
	rm windows-wlan-util.exe
