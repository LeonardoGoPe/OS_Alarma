/*Gomez Pesantes Leonardo David*/


/*Indicaciones para ejecutar Proyecto*/

1.- Generar los ejecutables con el Makefile adjunto

2.- Utilizar el formato ./proyecto [TIEMPO] [ARCHIVO]
/**/ El tiempo debe estar dado en segundos, hice así para mejor visualización de resultados. Preferible utilizar un tiempo mayor a 5 segundos para que se puedan generar los datos respectivos.
/**/ El archivo debe tener un formato como el indicado: id,sensorTipo,threshold. Los datos deben estar correctamente separadospor comas y con sus respectivos saltos de línea.

3.- Se puede verificar que se crean los procesos de acuerdo a la cantidad de sensores indicados en el archivo (ps -ax)

4.- Los procesos que generan sensores se comunican con los lectores por medio de memoria compartida. Esta memoria empieza desde 2345 y aumenta en uno para el siguiente proceso, para mayor facilidad y control para compartir la información

5.- Existen n procesos que generan sensores y n procesos que leen y realizan calculos (n = cantidad de sensores del archivo ingresado)

6.- Se crea un proceso en donde se realizan los cálculos y comparaciones para el resultados final

7.- **IMPORTANTE** Estoy conciente que la forma en que se realizan los cálculos no es la adecuada. Los datos obtenidos (id,tiposensor,media,varianza,threshold) se guardan en un archivo y ese archivo es leido por el proceso indicado anteriormente para realizar la verficiación de sensores.

8.- Tengo 5 sensores en la presentación por pantalla dado a que 4 serían competitivos (elijo siempre al de menor varianza) y 1 cooperativo (sería la acumulación de todos los que su tipo sea >4). En caso que no hayan sensores de un tipo, simplemente se presentan sus datos con 0, menos la varianza que asigné un valor X, esto para realizar la selección del sensor con menor varianza de manera rápida y además que se encuentra "activo" para facilitar la comparación.

9.- En caso que se haya elegido un lapso de tiempo N para imprimir resultados y el proceso se haya dormido o detenido en un lapso menor a ese, por ejemplo N-3 se detuvo la ejecución, el sensor se desactivará para el siguiente comparación y no para la actual, por lo que en esta iteración, el sensor si será considerado para el calculo.
