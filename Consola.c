#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h> 
#include <sys/shm.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/msg.h>
#include <ncurses.h> 

#define SHMSZ     4  

struct formato{
	char *id;
	char *tipoSensor;
	int threshold;
};

struct mesg_bufferA { 
	long tipoMensaje; 
	char textoMensaje[512]; 
} mensajeA; 

struct mesg_bufferB { 
	long tipoMensaje; 
	char textoMensaje[512]; 
} mensajeB; 

struct mesg_bufferC { 
	long tipoMensaje; 
	char textoMensaje[512]; 
} mensajeC; 

int commA = 7788;
int commB = 7799;
int commC = 7700;

int main(int argc, char *argv[]){
	while(1){
		int ch;
		initscr();			
		raw();				
		keypad(stdscr, TRUE);		
		noecho();			
		scrollok(stdscr, TRUE);
		cbreak();
		printw("\n********Seleccione la opción que desea ejecutar********\n");
		printw("A) Datos que llegan desde cada uno de los sensores\n");
		printw("B) Reglas que se cumplen y el estado de la alarma\n");
		printw("C) Ver información de diagnóstico de sensor\n");
		printw("D) Salir\n");

		printw("Por favor seleccione la opción que desea ejecutar: \n");
		ch = getch();

		if(ch==97){
			bool opcionA = true;
			printw("Seleccionó la opción A, presione *Enter* para ejecutar o X para salir\n");
			/*Memoria compartida para validar opcion A*/
			int shmidA,*shmA;
			if ((shmidA = shmget(commA, SHMSZ, IPC_CREAT | 0666)) < 0) {
				perror("shmget");
				return(1);
			}
			if ((shmA = shmat(shmidA, NULL, 0)) == (int *) -1) {
				perror("shmat");
				return(1);
			}
			if (getch() == 'x') {
				printw("Usted ha presionado X, presione *Enter* para salir\n");
				*shmA=0;
				opcionA=false;
			}
			*shmA=1;
			refresh();
			while(opcionA){
				nodelay(stdscr, TRUE); /* Para ejecutar sin interrupciones */
				key_t key; 
				int msgid; 

				key = ftok("proyectoA", 102); /*ftok para generar una unica llave*/
				msgid = msgget(key, 0666 | IPC_CREAT); /*msgget crear cola de mensajes y regresar identificador*/
				msgrcv(msgid, &mensajeA, sizeof(mensajeA), 1, 0);
				printw("%s \n",mensajeA.textoMensaje);
				if (getch() == 'x') {
					printw("Usted ha presionado X, presione *Enter* para salir\n");
					*shmA=0;
					opcionA=false;
				}
				refresh();			/* Actualizar pantalla */
				msgctl(msgid, IPC_RMID, NULL);  /* Eliminar cola de mensajes*/
			}
			close(shmidA);

		}else if(ch==98){
			bool opcionB = true;
			printw("Seleccionó la opción B, presione *Enter* para ejecutar o X para salir\n");
			/*Memoria compartida para validar opcion B*/
			int shmidB,*shmB;
			if ((shmidB = shmget(commB, SHMSZ, IPC_CREAT | 0666)) < 0) {
				perror("shmget");
				return(1);
			}
			if ((shmB = shmat(shmidB, NULL, 0)) == (int *) -1) {
				perror("shmat");
				return(1);
			}
			if (getch() == 'x') {
				printw("Usted ha presionado X, presione *Enter* para salir\n");
				*shmB=0;
				opcionB=false;
			}
			*shmB=1;
			refresh();
			while(opcionB){
				nodelay(stdscr, TRUE); /* Para ejecutar sin interrupciones */
				key_t keyB; 
				int msgidB; 

				keyB = ftok("proyectoB", 103); /*ftok para generar una unica llave*/

				msgidB=msgget(keyB, 0666 | IPC_CREAT);/*msgget crear cola de mensajes y regresar identificador*/
				msgrcv(msgidB, &mensajeB, sizeof(mensajeB), 1, 0);
				printw("%s \n",mensajeB.textoMensaje);
				if (getch() == 'x') {
					printw("Usted ha presionado X, presione *Enter* para salir\n");
					*shmB=0;
					opcionB=false;
				}
				refresh();			/* Actualizar pantalla */
				msgctl(msgidB, IPC_RMID, NULL); /* Eliminar cola de mensajes*/
			}
			close(shmidB);
			
		}else if(ch==99){
			bool opcionC = true;
			printw("Seleccionó la opción C, presione *Enter* para ejecutar o X para salir\n");
			/*Memoria compartida para validar opcion C*/
			int shmidC,*shmC;
			if ((shmidC = shmget(commC, SHMSZ, IPC_CREAT | 0666)) < 0) {
				perror("shmget");
				return(1);
			}
			if ((shmC = shmat(shmidC, NULL, 0)) == (int *) -1) {
				perror("shmat");
				return(1);
			}
			if (getch() == 'x') {
				printw("Usted ha presionado X, presione *Enter* para salir\n");
				*shmC=0;
				opcionC=false;
			}
			*shmC=1;
			refresh();
			while(opcionC){
				nodelay(stdscr, TRUE); /* Para ejecutar sin interrupciones */
				key_t key; 
				int msgid; 
				key = ftok("proyectoC", 104); /*ftok para generar una unica llave*/
				msgid=msgget(key, 0666 | IPC_CREAT); /*msgget crear cola de mensajes y regresar identificador*/
				msgrcv(msgid, &mensajeC, sizeof(mensajeC), 1, 0);
				printw("%s \n",mensajeC.textoMensaje);
				if (getch() == 'x') {
					printw("Usted ha presionado X, presione *Enter* para salir\n");
					*shmC=0;
					opcionC=false;
				}
				refresh();			/* Actualizar pantalla */
				msgctl(msgid, IPC_RMID, NULL); /* Eliminar cola de mensajes*/
			}
			close(shmidC);
		}else if(ch==100){
			endwin();			/* Salir de modo curses */
			return 0;
		}else{
			printf("Opcion inválida\n");
		}
		nodelay(stdscr, FALSE);
		refresh();			/* Actualizar pantalla */
	    	getch();			/* Esperar por ingreso de usuario */
		endwin();			/* Salir de modo curses */
	}
	return 0;

}
