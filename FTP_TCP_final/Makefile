all: clients/client servers/server
%: %.cc
	g++ -std=c++11 $< -w -o $@

%: %.c
	gcc $< -w -o $@ -pthread
clean:
	rm -f clients/client servers/server
