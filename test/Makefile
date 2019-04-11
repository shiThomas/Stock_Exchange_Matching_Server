TARGETS=client server

all: $(TARGETS)
clean:
	rm -f $(TARGETS)

player: client.cpp 
	g++ -g -o $@ $<

master: server.cpp 
	g++ -g -o $@ $<




