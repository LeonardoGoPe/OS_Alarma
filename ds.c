#include <ncurses.h>


key_t keyOpA; 
int msgidOpA; 
keyOpA = ftok("proyectoOpA", 120);
msgidOpA = msgget(keyOpA, 0666 | IPC_CREAT);
senialOpA.tipoMensaje = 1;
strcat(senialOpA.textoMensaje,"1");
msgsnd(msgidOpA, &senialOpA, sizeof(senialOpA), 0);


int
main()
{
    initscr();
    cbreak();
    noecho();
    scrollok(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    bool valor = true;
    while (valor) {
        if (getch() == 'g') {
            printw("You pressed G\n");
	    valor=false;
        }
        napms(500);
        printw("Running\n");
    }
    printw("GG");
    refresh();			/* Print it on to the real screen */
    //endwin();			/* End curses mode		  */
}
