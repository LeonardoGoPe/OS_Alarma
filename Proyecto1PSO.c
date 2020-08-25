/*Gomez Pesantes Leonardo David*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h> 
#include <sys/shm.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <math.h>
#include <sys/msg.h> 

                                                          
#define SIZE 50
#define MAX_SIZE 1024
#define SHMSZ     4                                                         

struct formato{
	char *id;
	char *tipoSensor;
	int threshold;
};

struct Tiempos{
	int id;
	int tipoSensor;
	int tiempo;
	int cantidadDatos;
	int valores[1024];
};

struct sen_format{
	int id;
	float media;
	float varianza;
	int threshold;
	int encendida;
};

struct valoresSensores{
	int id;
	int tipoSensor;
	float media;
	float varianza;
	int threshold;
	int estado;
};

struct mesg_buffer { 
	long tipoMensaje; 
	char textoMensaje[512]; 
} mensaje; 

struct mesg_buffer_end { 
	long tipoMensaje; 
	char textoMensaje[512]; 
} mensajeRecibido; 

int memCompartidaInicial = 2345; /*Primera dirección desde donde se compartirá memoria. Los demás sensores compartirán la siguiente*/

/*Metodo para crear procesos indicados en el archivo*/
void creacionSensores(void *arg, char *argId, char *argTipoSensor){ 
	int i = *(int *) arg;
	int  val = memCompartidaInicial+i ,numMin, numMax;
	char buffPuerto[8], buffMin[4], buffMax[4];
	numMin = 20+i;
	numMax = 80-i;

	sprintf(buffPuerto,"%d",val);
	sprintf(buffMin,"%d",numMin);
	sprintf(buffMax,"%d",numMax);

	char *sensor = "./sx";
	char *argv[9];
	argv[0] = "./sx";
	argv[1] = argId;
	argv[2] = argTipoSensor;
	argv[3] = buffPuerto;
	argv[4] = "1000";
	argv[5] = buffMin;
	argv[6] = buffMax;
	argv[7] = "1";
	argv[8] = NULL;
	execvp(sensor,argv);
}

/*Metodo para leer los datos en otro proceso por medio de memoria compartida con los procesos de sensores y realizar los cálculos respectivos. Los resultados escriben en un archivo*/
void lecturaSensores(void *arg, char *argId, char *argTipoSensor, int argTh, int tiempo){

	struct Tiempos datosCalculos = {0, 0, 0, 0, 0};

	int i = *(int *) arg;
	int comm = memCompartidaInicial+i;
	char commC[8];
	int shmid,*shm;
	
	int shmidRes;
	int *shmRes, *s;

	int sumatoria, contador;

	char idSensor[5];
	char tipoSensorF[5];
	char mediaSensor[16];
	char varianzaSensor[16];
	char thresholdSensor[16];
	char estadoSensor[5];

	sprintf(commC,"%d",comm);
	datosCalculos.id=atoi(argId);
	datosCalculos.tipoSensor=atoi(argTipoSensor);

	if ((shmid = shmget(comm, SHMSZ,  0666)) < 0) {
		perror("shmget");
		return(1);
	}
	if ((shm = shmat(shmid, NULL, 0)) == (int *) -1) {
		perror("shmat");
		return(1);
	}
	while(1){
		if (*shm==-1){
			printf("Sensor %s detenido\n",argId);
			close(shmid);
			break;
		}
		datosCalculos.valores[contador]=*shm;
		sumatoria += *shm;
		contador++;
		datosCalculos.tiempo = sumatoria;
		datosCalculos.cantidadDatos = contador;

		if(contador == tiempo){

			float media = 0, varianza = 0;
			int estado;
			media = (float)datosCalculos.tiempo/(float)datosCalculos.cantidadDatos;
			for(int k = 0; k < contador; k++){
				varianza += pow(((float) datosCalculos.valores[k] - media),2);
			}
			if(varianza==0.0){
				estado = 0;
			}else{
				estado = 1;
			}
			varianza = varianza/(float)datosCalculos.cantidadDatos;

			/*Almacenar valores en variables char*/
			sprintf(idSensor,"%d",datosCalculos.id);
			sprintf(tipoSensorF,"%d",datosCalculos.tipoSensor);
			sprintf(mediaSensor,"%0.2f",media);
			sprintf(varianzaSensor,"%0.2f",varianza);
			sprintf(thresholdSensor,"%d",argTh);
			sprintf(estadoSensor,"%d",estado);
 
			/*Creacion de la cola de mensajes*/
			key_t key; 
			int msgid; 
			key = ftok("proyecto", 101);
			msgid = msgget(key, 0666 | IPC_CREAT);
			mensaje.tipoMensaje = 1;

			/*Agregar string al arreglo del mensaje para IPC*/
			strcat(mensaje.textoMensaje,idSensor);
			strcat(mensaje.textoMensaje,",");
			strcat(mensaje.textoMensaje,tipoSensorF);
			strcat(mensaje.textoMensaje,",");
			strcat(mensaje.textoMensaje,mediaSensor);
			strcat(mensaje.textoMensaje,",");
			strcat(mensaje.textoMensaje,varianzaSensor);
			strcat(mensaje.textoMensaje,",");
			strcat(mensaje.textoMensaje,thresholdSensor);
			strcat(mensaje.textoMensaje,",");
			strcat(mensaje.textoMensaje,estadoSensor);

			msgsnd(msgid, &mensaje, sizeof(mensaje), 0);
			printf("Dato enviados de sensores: %s \n", mensaje.textoMensaje);

			sumatoria = 0;
			contador = 0;
			contador = 0;
			strcpy(mensaje.textoMensaje,"");
		}
		sleep(1);
	}
}


/*Metodo que realiza los procesos para determinar si se enciende o no la alarma. Se lee el archivo generado*/
void calculos(int sensoresIngresados){
	int i = 0;
	int isSenCoop=0, numSenCoop=0;
	float senCoop;
	char *ptr = NULL;

	struct valoresSensores datosCalculos = {0, 0, 0.0, 0.0, 0, 0};

	struct sen_format s1 = {0,0.0,(float)MAX_SIZE,0,1};
	struct sen_format s2 = {0,0.0,(float)MAX_SIZE,0,1};
	struct sen_format s3 = {0,0.0,(float)MAX_SIZE,0,1};
	struct sen_format s4 = {0,0.0,(float)MAX_SIZE,0,1};

	key_t key; 
	int msgid; 

	// ftok to generate unique key 
	key = ftok("proyecto", 101); 

	// msgget creates a message queue 
	// and returns identifier 
	msgid = msgget(key, 0666 | IPC_CREAT); 

	for(int k = 0; k <=sensoresIngresados; k++){
		// msgrcv to receive message
		msgrcv(msgid, &mensajeRecibido, sizeof(mensajeRecibido), 1, 0); 

		// display the message 
		//printf("Datos recibidos por Calculos : %s \n", mensajeRecibido.textoMensaje);
		ptr = strtok(mensajeRecibido.textoMensaje, ",");
		datosCalculos.id=atoi(ptr);
		while (NULL !=(ptr = strtok(NULL,","))){
			i++;
			if(i==1)
				datosCalculos.tipoSensor = atoi(ptr);
			else if(i==2)
				datosCalculos.media = atof(ptr);
			else if(i==3)
				datosCalculos.varianza = atof(ptr);
			else if(i==4)
				datosCalculos.threshold = atoi(ptr);
			else if(i==5)
				datosCalculos.estado = atoi(ptr);
		}
		//printf("Datos recibidos por Calculos : %d-%d-%0.2f-%0.2f-%d-%d \n",datosCalculos.id,datosCalculos.tipoSensor, datosCalculos.media, datosCalculos.varianza, datosCalculos.threshold, datosCalculos.estado);
		i=0;
		if(datosCalculos.tipoSensor == 1){
			if(datosCalculos.varianza < s1.varianza){
				s1.id = datosCalculos.id;
				s1.media = datosCalculos.media;
				s1.varianza = datosCalculos.varianza;
				s1.threshold = datosCalculos.threshold;
			}
		}
		if(datosCalculos.tipoSensor == 2){
			if(datosCalculos.varianza < s2.varianza){
				s2.id = datosCalculos.id;
				s2.media = datosCalculos.media;
				s2.varianza = datosCalculos.varianza;
				s2.threshold = datosCalculos.threshold;
			}
		}
		if(datosCalculos.tipoSensor == 3){
			if(datosCalculos.varianza < s3.varianza){
				s3.id = datosCalculos.id;
				s3.media = datosCalculos.media;
				s3.varianza = datosCalculos.varianza;
				s3.threshold = datosCalculos.threshold;
			}
		}
		if(datosCalculos.tipoSensor == 4){
			if(datosCalculos.varianza < s4.varianza){
				s4.id = datosCalculos.id;
				s4.media = datosCalculos.media;
				s4.varianza = datosCalculos.varianza;
				s4.threshold = datosCalculos.threshold;
			}
		}
		if(datosCalculos.tipoSensor > 4){
			if(datosCalculos.estado == 1){
				if(datosCalculos.media > (float) datosCalculos.threshold){
					isSenCoop++;
				}
				numSenCoop++;
			}
		}
	}
	if((float)s1.media < (float)s1.threshold){
		s1.encendida=0;
	}
	if((float)s2.media < (float)s2.threshold){
		s2.encendida=0;
	}
	if((float)s3.media < (float)s3.threshold){
		s3.encendida=0;
	}
	if((float)s4.media < (float)s4.threshold){
		s4.encendida=0;
	}
	if(numSenCoop==0){
		senCoop=0;
	}else{
		senCoop = (float)isSenCoop/(float)numSenCoop;
	}
	printf("Sensor 1 -- Id: %d, Media: %0.2f, Varianza: %0.2f, Threshold: %d, Encendido: %d\n",s1.id,s1.media,s1.varianza,s1.threshold,s1.encendida);
	printf("Sensor 2 -- Id: %d, Media: %0.2f, Varianza: %0.2f, Threshold: %d, Encendido: %d\n",s2.id,s2.media,s2.varianza,s2.threshold,s2.encendida);
	printf("Sensor 3 -- Id: %d, Media: %0.2f, Varianza: %0.2f, Threshold: %d, Encendido: %d\n",s3.id,s3.media,s3.varianza,s3.threshold,s3.encendida);
	printf("Sensor 4 -- Id: %d, Media: %0.2f, Varianza: %0.2f, Threshold: %d, Encendido: %d\n",s4.id,s4.media,s4.varianza,s4.threshold,s4.encendida);
	printf("Sensores Cooperativos: %f\n",senCoop);
	if(s1.encendida && s2.encendida && s3.encendida && s4.encendida && ((float)senCoop > 0.5)){
		printf("Alarma encendida!\n\n");
	}else{
		printf("Alarma apagada!\n\n");
	}	
	// to destroy the message queue 
	msgctl(msgid, IPC_RMID, NULL); 
}

int main(int argc, char *argv[]){

	/*Validacion de ingreso de datos*/
	if (argc != 3){ 
        	printf("Ingrese solo 2 argumentos Ej: ./proyecto 10 archivo.txt\n"); 
        	return 0; 
    	} 

	int tiempo = atoi(argv[1]);
	char *archivo = argv[2];

	FILE *fp = NULL;                                                            
	int i = 0, j, controlProcesos = 0;                                                                  
	struct formato var = {NULL, NULL, 0};                                
	char line[SIZE] = {0}, *ptr = NULL;

	if (NULL == (fp = fopen(archivo,"r"))){ /*Lectura del archivo ingresado por la consola*/
		perror("Error");
		exit(EXIT_FAILURE);
	}

	var.id = malloc(SIZE);
	var.tipoSensor = malloc(SIZE);


	while(EOF != fscanf(fp, "%s",line)){
		ptr = strtok(line, ",");

		var.id=ptr;

		while (NULL !=(ptr = strtok(NULL,","))){
			i++;

			if(i==1)
				var.tipoSensor = ptr;
			else if(i==2)
				var.threshold = atoi(ptr);
		}
		i = 0;
		pid_t cpidCrear, cpidLeer; 
		cpidCrear = fork(); /*Creacion de procesos para generar datos*/
		if(cpidCrear == -1){
			perror("fork");
			exit(EXIT_FAILURE);
		} else if (cpidCrear == 0){
			usleep(controlProcesos*1000);
			creacionSensores(&controlProcesos, var.id, var.tipoSensor);
			exit(0);
		} 

		cpidLeer = fork(); /*Creacion de procesos para leer datos y trabajar con ellos*/
		if(cpidLeer == -1){
			perror("fork");
			exit(EXIT_FAILURE);
		} else if (cpidLeer == 0){
			sleep(1);
			lecturaSensores(&controlProcesos, var.id, var.tipoSensor, var.threshold, tiempo);
			exit(0);
		}
		controlProcesos++;
	}
	fclose(fp);

	pid_t cpidCalculos;
	cpidCalculos = fork(); /*Creacion de procesos que realiza los cálculos para indicar si se enciende o no la alarma*/
	if(cpidCalculos == -1){
		perror("fork");
		exit(EXIT_FAILURE);
	} else if (cpidCalculos == 0){
		while(1){
			sleep(tiempo+0.1);
			printf("La cantidad de procesos son %d\n",controlProcesos);
			calculos(controlProcesos-1);
		}
		exit(0);
	}
	for(j=0;j<(controlProcesos*2);j++){
	    wait(NULL); 
	}
	return 0;
}
