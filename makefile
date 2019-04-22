all: jit

jit:
	g++ jit.cpp -o jit

run: all
	./jit
	
clean:
	rm -f jit
