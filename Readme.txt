/*Gomez Pesantes Leonardo David*/


/*Indicaciones para ejecutar Proyecto*/

Formatos:
Para ejecutar el lector: ./proyecto [TIEMPO] [ARCHIVO] ; Tiempo dado en segundos y Archivo el de pruebas
Para ejecutar la consola: ./consola

El archivo debe tener el formato: id,tipoSensor,Threshold,DireccionMemoria

1.- Generar los ejecutables con el Makefile adjunto
Nota: asegurarse de tener instalada la libreria ncurses

2.- Se puede verificar que se crean los lectores de sensores respectivos (ps -ax)

3.- Los sensores se generan mediante el Script respectivo.
Nota: asegurarse de que si se desea agregar un nuevo sensor al Script, se agregue también al [ARCHIVO] la direccion de memoria

4.- Se crea un proceso en donde se realizan los cálculos y comparaciones para el resultados final

5.- Todos los procesos se comunican mediante Memoria compartida o Colas de mensajes. Cada una independiente para una determinada tarea

6.- Tengo 5 sensores en la presentación por pantalla dado a que 4 serían competitivos (elijo siempre al de menor varianza) y 1 cooperativo (sería la acumulación de todos los que su tipo sea >4). En caso que no hayan sensores de un tipo, simplemente se presentan sus datos con 0, menos la varianza que asigné un valor X, esto para realizar la selección del sensor con menor varianza de manera rápida y además que se encuentra "activo" para facilitar la comparación.

7.- El comando Ctrl+C está desactivado en la consola para que se asegure de que las colas se cierran correctamente.


/**Importante: ejecutar el proyecto primero, debido a que sin esto, la consola no tendrá información de donde leer y se quedará congelada, al menos al momento de elegir una opción, esto es por el manejo de las librerías ncurses, que no pude manejarla del todo. De igual manera, asegurarse de salir correctamente de las opciones
