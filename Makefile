build:
	gcc main.c -o main -lX11 -g3
	gcc event.c -o event -lX11 -g3

clean:
	rm -f event main
