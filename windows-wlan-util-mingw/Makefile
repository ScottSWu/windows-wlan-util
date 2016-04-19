CC=g++
CFLAGS=-std=c++11
LIBS=-lwlanapi -lole32
STATIC_LIBS=

windows-wlan-util.exe: windows-wlan-util.cpp
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

clean:
	rm windows-wlan-util.exe
