all:
	gcc -o sensorx sensorx.c
	gcc -o proyecto Proyecto2PSO.c -lm
	gcc -o consola Consola.c -lncurses
