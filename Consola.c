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

int commA = 7788;
int commB = 7799;
int commC = 7700;

int main(int argc, char *argv[]){
	while(1){
		int ch;
		initscr();			/* Start curses mode 		*/
		raw();				/* Line buffering disabled	*/
		keypad(stdscr, TRUE);		/* We get F1, F2 etc..		*/
		noecho();			/* Don't echo() while we do getch */
		scrollok(stdscr, TRUE);
		cbreak();
		printw("********Seleccione la opción que desea ejecutar********\n");
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
			int shmid,*shm;
			if ((shmid = shmget(commA, SHMSZ, IPC_CREAT | 0666)) < 0) {
				perror("shmget");
				return(1);
			}
			if ((shm = shmat(shmid, NULL, 0)) == (int *) -1) {
				perror("shmat");
				return(1);
			}
			if (getch() == 'x') {
				printw("Usted ha presionado X, presione *Enter* para salir\n");
				*shm=0;
				opcionA=false;
			}
			*shm=1;
			refresh();
			while(opcionA){
				nodelay(stdscr, TRUE); /* Para ejecutar sin interrupciones */
				key_t key; 
				int msgid; 

				// ftok para generar una unica llave
				key = ftok("proyectoA", 102); 

				// msgget creates a message queue 
				// and returns identifier 
				msgid = msgget(key, 0666 | IPC_CREAT); 
				for(int k = 0; k<=9;k++){
					msgrcv(msgid, &mensajeA, sizeof(mensajeA), 1, 0);
					printw("%s \n",mensajeA.textoMensaje);
				}
				if (getch() == 'x') {
					printw("Usted ha presionado X, presione *Enter* para salir\n");
					*shm=0;
					opcionA=false;
				}
				refresh();			/* Print it on to the real screen */

				// to destroy the message queue 
				msgctl(msgid, IPC_RMID, NULL);
			}
			close(shmid);

		}else if(ch==98){
			bool opcionB = true;
			printw("Seleccionó la opción B, presione *Enter* para ejecutar o X para salir\n");
			/*Memoria compartida para validar opcion A*/
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

				// ftok para generar una unica llave
				keyB = ftok("proyectoB", 103); 

				// msgget creates a message queue 
				// and returns identifier 
				msgidB = msgget(keyB, 0666 | IPC_CREAT); 
				msgrcv(msgidB, &mensajeB, sizeof(mensajeB), 1, 0);
				printw("%s \n",mensajeB.textoMensaje);
				if (getch() == 'x') {
					printw("Usted ha presionado X, presione *Enter* para salir\n");
					*shmB=0;
					opcionB=false;
				}
				refresh();			/* Print it on to the real screen */

				// to destroy the message queue 
				msgctl(msgidB, IPC_RMID, NULL);
			}
			close(shmidB);
			
		}else if(ch==99){
			printw("Usted seleccionó la opción C\n");
			nodelay(stdscr, TRUE); /* Para ejecutar sin interrupciones */
			if (getch() == 'x') {
				printw("Usted ha presionado X, presione *Enter* para salir\n");
			}
			refresh();
		}else if(ch==100){
			endwin();			/* End curses mode		  */
			return 0;
		}else{
			printf("Opcion inválida\n");
		}
		nodelay(stdscr, FALSE);
		refresh();			/* Print it on to the real screen */
	    	getch();			/* Wait for user input */
		endwin();			/* End curses mode		  */
	}
	return 0;

}
