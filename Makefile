all:
	gcc -o sensorx sensorx.c
	gcc -o proyecto Proyecto1PSO.c -lm
	gcc -o consola Consola.c -lncurses
