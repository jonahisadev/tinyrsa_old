all: tinyrsa

install: tinyrsa
	sudo cp tinyrsa /usr/local/bin

tinyrsa: tinyrsa.cpp
	g++ tinyrsa.cpp -o tinyrsa -lbigint -std=c++11