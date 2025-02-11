#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h> //para el sleep

//VARIABLES GLOBALES

//configuracion inicial
int habitantes;
int centros_vacunacion;
int fabricas;
int vacunas_tanda;
int vacunas_iniciales;//las vacunas que hay en los centros antes de que las fabricas les manden más
int vacunas_fabrica; //las vacunas que debe fabricar cada fabrica
int min_vacunas_tanda; //minimo de vacunas fabricadas en cada tanda
int max_vacunas_tanda;
int t_min_fabricacion_tanda;//tiempo minimo de fabricacion de una tanda de vacunas
int t_max_fabricacion_tanda;
int t_max_reparto;//tiempo maximo de reparte de las vacunas en lso centros
int t_max_cita; //tiempo maximo que tarda la persona en ser citada par vacunarse
int t_max_desplazamiento; //tiempo maximo de desplazamiento de la persona al centro para vacunarse

// Estructuras para estadísticas
typedef struct {
    int fabricadas;           // Vacunas fabricadas por la fábrica
    int *entregadas_a_centros; // Array dinámico: Vacunas entregadas por la fábrica a cada centro
} Fabrica;

typedef struct {
    int recibidas; // Vacunas recibidas en total por el centro
    int *habitantesVacunados; // Array de habitantes vacunados en el centro
    int vacunados;
    int personas_esperando; //personas esperando en el centro para ser vacunadas
    pthread_cond_t cond;  // Condición de espera para habitantes
    int disponibles;//las disponibles son las sobrantes
} CentroVacunacion;

// Arrays dinámicos para estadísticas
Fabrica *fabricas_estadisticas;
CentroVacunacion *centros_estadisticas;

pthread_mutex_t mutex; //creamos un mutex, para proteger los recursos compartidos

void variables(FILE *fd){//guardar el contenido del fichero de entrada en variables

    fscanf(fd,"%i\n",&habitantes);
    centros_vacunacion=5;
    fabricas=3;
    vacunas_tanda=habitantes/10;
    fscanf(fd,"%i\n",&vacunas_iniciales);
    vacunas_fabrica=habitantes/fabricas; 
    fscanf(fd,"%i\n",&min_vacunas_tanda);
    fscanf(fd,"%i\n",&max_vacunas_tanda);
    fscanf(fd,"%i\n",&t_min_fabricacion_tanda);
    fscanf(fd,"%i\n",&t_max_fabricacion_tanda);
    fscanf(fd,"%i\n",&t_max_reparto);
    fscanf(fd,"%i\n",&t_max_cita);
    fscanf(fd,"%i\n",&t_max_desplazamiento);

    // Asignación dinámica de memoria para estadísticas
    fabricas_estadisticas = (Fabrica *)malloc(fabricas * sizeof(Fabrica));
    centros_estadisticas = (CentroVacunacion *)malloc(centros_vacunacion * sizeof(CentroVacunacion));

    // Inicialización de las estructuras
    for (int i = 0; i < fabricas; i++) {
        fabricas_estadisticas[i].fabricadas = 0;
        fabricas_estadisticas[i].entregadas_a_centros = (int *)malloc(centros_vacunacion * sizeof(int));
        for (int j = 0; j < centros_vacunacion; j++) {
            fabricas_estadisticas[i].entregadas_a_centros[j] = 0;
        }
    }

    for (int i = 0; i < centros_vacunacion; i++) {
        centros_estadisticas[i].recibidas = vacunas_iniciales;//al principio todos los centros recibiran las vacunas iniciales
        centros_estadisticas[i].habitantesVacunados = (int *)malloc(habitantes* sizeof(int));
        centros_estadisticas[i].vacunados = 0;
        centros_estadisticas[i].disponibles = vacunas_iniciales;
        centros_estadisticas[i].personas_esperando = 0;
        pthread_cond_init(&centros_estadisticas[i].cond, NULL);  // Inicializar condición para cada centro
    }

}

void imprimirConfiguracionInicial(FILE *salida){//imprimir en pantalla y escribirlo en el fichero de salida

    //imprimir por pantalla
    printf("VACUNACION EN PANDEMIA: CONFIGURACIÓN INICIAL\n");
    printf("Habitantes: %i\n",habitantes);
    printf("Centros de vacunación: %i\n",centros_vacunacion);
    printf("Fábricas: %i\n",fabricas);
    printf("Vacunados por tanda: %i\n",vacunas_tanda);
    printf("Vacunas iniciales en cada centro: %i\n",vacunas_iniciales);
    printf("Vacunas totales por fábrica: %i\n",vacunas_fabrica);
    printf("Mínimo número de vacunas fabricadas en cada tanda: %i\n",min_vacunas_tanda);
    printf("Máximo número de vacunas fabricadas en cada tanda: %i\n",max_vacunas_tanda);
    printf("Tiempo mínimo de fabricación de una tanda de vacunas: %i\n",t_min_fabricacion_tanda);
    printf("Tiempo máximo de fabricación de una tanda de vacunas: %i\n",t_max_fabricacion_tanda);
    printf("Tiempo máximo de reparto de vacunas a los centros: %i\n",t_max_reparto);
    printf("Tiempo máximo que un habitante tarda en ver que está citado para vacunarse: %i\n",t_max_cita);
    printf("Tiempo máximo de desplazamiento del habitante al centro de vacunación: %i\n",t_max_desplazamiento);

    //escribir en el fichero de salida
    fprintf(salida,"VACUNACION EN PANDEMIA: CONFIGURACIÓN INICIAL\n");
    fprintf(salida,"Habitantes: %i\n",habitantes);
    fprintf(salida,"Centros de vacunación: %i\n",centros_vacunacion);
    fprintf(salida,"Fábricas: %i\n",fabricas);
    fprintf(salida,"Vacunados por tanda: %i\n",vacunas_tanda);
    fprintf(salida,"Vacunas iniciales en cada centro: %i\n",vacunas_iniciales);
    fprintf(salida,"Vacunas totales por fábrica: %i\n",vacunas_fabrica);
    fprintf(salida,"Mínimo número de vacunas fabricadas en cada tanda: %i\n",min_vacunas_tanda);
    fprintf(salida,"Máximo número de vacunas fabricadas en cada tanda: %i\n",max_vacunas_tanda);
    fprintf(salida,"Tiempo mínimo de fabricación de una tanda de vacunas: %i\n",t_min_fabricacion_tanda);
    fprintf(salida,"Tiempo máximo de fabricación de una tanda de vacunas: %i\n",t_max_fabricacion_tanda);
    fprintf(salida,"Tiempo máximo de reparto de vacunas a los centros: %i\n",t_max_reparto);
    fprintf(salida,"Tiempo máximo que un habitante tarda en ver que está citado para vacunarse: %i\n",t_max_cita);
    fprintf(salida,"Tiempo máximo de desplazamiento del habitante al centro de vacunación: %i\n",t_max_desplazamiento);


}
void distribuir_vacunas(int cantidadVacunas,int id_fabrica){
    int total_personas_esperando = 0;
    //int *centros_a_despertar = (int *)calloc(centros_vacunacion, sizeof(int)); // Array para saber qué centros despertar
    int vacunas_asignadas;

    pthread_mutex_lock(&mutex); //bloqueamos el mutex para que ningún otro hilo pueda entrar a la sección crítica hasta que el primero haya terminado
    // Calcular el total de personas esperando en todos los centros
    for (int i = 0; i < centros_vacunacion; i++) {
        total_personas_esperando += centros_estadisticas[i].personas_esperando;
    }

    //REPARTIR SEGUN LA DEMANDA, PARA CUBRIR LAS NECESIDADES Y CON LAS VACUNAS SOBRANTES REPARTIRLAS DE MANERA PROPORCIONAL,usando 2 bucles for
    // Distribuir las vacunas en función de la demanda de cada centro
    for (int i = 0; i < centros_vacunacion; i++) {
        // Si hay personas esperando en el centro, se distribuyen vacunas proporcionalmente, en base a la demanda
        if (centros_estadisticas[i].personas_esperando > 0){
            vacunas_asignadas = (cantidadVacunas * centros_estadisticas[i].personas_esperando) / total_personas_esperando;
            

            if (vacunas_asignadas > 0) { // Si se asignan vacunas, 
                pthread_cond_broadcast(&centros_estadisticas[i].cond); // Notificar a los habitantes
            }

            centros_estadisticas[i].recibidas += vacunas_asignadas;//son las totales recibidas, estas solo suben
            centros_estadisticas[i].disponibles +=vacunas_asignadas;//las disponibles en el centro, suben y bajan
            fabricas_estadisticas[id_fabrica].entregadas_a_centros[i] += vacunas_asignadas;
            cantidadVacunas -= vacunas_asignadas;

            ////MENSAJE DE DISTRIBUIR VACUNAS ANTES DEL BREAK
            printf("Fábrica %d entrega %d vacunas al Centro %d.\n" , id_fabrica +1,vacunas_asignadas,i+1);

            if (cantidadVacunas <= 0) 
                break; // Salimos si ya no quedan vacunas
             
        
        }
    }
    if(cantidadVacunas > 0){//repartir las vacunas sobrantes de manera equitativa entre los centros
        vacunas_asignadas=cantidadVacunas / centros_vacunacion;
        int resto = cantidadVacunas % centros_vacunacion; // Calculamos el resto de la división

        // Asignamos a todos los centros las vacunas básicas
        for(int i=0; i<centros_vacunacion; i++){

            if (vacunas_asignadas > 0) { // Si se asignan vacunas, 
                pthread_cond_broadcast(&centros_estadisticas[i].cond); // Notificar a los habitantes
            }

            centros_estadisticas[i].recibidas+=vacunas_asignadas;
            centros_estadisticas[i].disponibles +=vacunas_asignadas;
            fabricas_estadisticas[id_fabrica].entregadas_a_centros[i] += vacunas_asignadas;
            cantidadVacunas -= vacunas_asignadas;

            //MENSAJE DE DISTRIBUIR VACUNAS ANTES DEL BREAK
            printf("Fábrica %d entrega %d vacunas al Centro %d.\n" , id_fabrica +1,vacunas_asignadas,i+1);

            if (cantidadVacunas <= 0) 
                break; // Salimos si ya no quedan vacunas

        }

        // Ahora repartimos el resto entre los primeros "resto" centros
        for (int i = 0; i < resto; i++) {
            if(resto>0){
                pthread_cond_broadcast(&centros_estadisticas[i].cond);
                // Asignamos 1 vacuna extra a los primeros "resto" centros
                centros_estadisticas[i].recibidas += 1;
                centros_estadisticas[i].disponibles += 1;
                fabricas_estadisticas[id_fabrica].entregadas_a_centros[i] += 1;
                cantidadVacunas -= 1;
            }
            
            // Imprimimos el detalle de la distribución del resto
            printf("Fábrica %d entrega 1 vacuna extra al Centro %d.\n", id_fabrica + 1, i + 1);

            if (cantidadVacunas <= 0) 
                break; // Salimos si ya no quedan vacunas
        }
    }

    pthread_mutex_unlock(&mutex); //liberamos mutex para que otros hilos puedan entrar en la sección crítica

}



void *fabrica_vacunas(void *arg){
    int id_fabrica = *((int*)arg);
    int total_producidas = 0;

    while (total_producidas < vacunas_fabrica) {
        int cantidad = (rand() % (max_vacunas_tanda - min_vacunas_tanda + 1)) + min_vacunas_tanda;
        //hacemos que la cantidad de vacunas en la tanda sea un numero aleatorio entre el numero de vacunas min_tanda y numero de vacunas max_tanda
        if (total_producidas + cantidad > vacunas_fabrica) {
            cantidad = vacunas_fabrica - total_producidas; //si la produccion de cantidad de vacunas excede el max de vacunas/fabrica ajustamos la cantidad
        }
        

        //Tiempo de fabricación
        int tiempo_fabricacion = (rand() % (t_max_fabricacion_tanda - t_min_fabricacion_tanda + 1)) + t_min_fabricacion_tanda;
        sleep(tiempo_fabricacion); //se detiene la ejecucion del hilo un numero aleatorio entre t_min_fabricacion_tanda y t_max_fabricacion_tanda
        
        // Actualizamos las estadísticas
        fabricas_estadisticas[id_fabrica].fabricadas += cantidad;

        //IMPRIMIR EL NUMERO DE VACUNAS PREPARADAS ANTES DE SU DISTRIBUCION
        total_producidas += cantidad; //para controlar que sea menor que vacunas_fabrica 
        printf("Fábrica %d prepara %d vacunas.\n", id_fabrica + 1,cantidad);
        //Repartimos las vacunas de la fabrica a los centros
        distribuir_vacunas(cantidad, id_fabrica); 

        
    }

    printf("Fábrica %d ha terminado su producción.\n", id_fabrica + 1);
    pthread_exit(NULL);
}

void *habitante_vacunacion(void *arg) {
    int id_habitante = *((int*)arg);
    int centro_vacunacion = rand() % centros_vacunacion;

    // Simulamos la llegada del habitante
    printf("Habitante %d elige el centro %d para vacunarse.\n", id_habitante + 1, centro_vacunacion + 1);

    pthread_mutex_lock(&mutex);
    // Incrementa el número de personas esperando en el centro
    centros_estadisticas[centro_vacunacion].personas_esperando++;
    pthread_mutex_unlock(&mutex);

    
    pthread_mutex_lock(&mutex);

    while(centros_estadisticas[centro_vacunacion].disponibles <= 0) {
        // Esperar hasta que haya vacunas disponibles
        pthread_cond_wait(&centros_estadisticas[centro_vacunacion].cond, &mutex);
    } 
            
    // Vacuna disponible, proceder con la vacunación
    printf("Habitante %d vacunado en el centro %d.\n", id_habitante + 1, centro_vacunacion + 1);
    centros_estadisticas[centro_vacunacion].habitantesVacunados[id_habitante] = 1;
    centros_estadisticas[centro_vacunacion].disponibles--;  
    centros_estadisticas[centro_vacunacion].vacunados++;
    centros_estadisticas[centro_vacunacion].personas_esperando--;
    
    // Actualizar sobrantes en el centro
    int sobrantes_centro = centros_estadisticas[centro_vacunacion].recibidas - centros_estadisticas[centro_vacunacion].vacunados;
    if (sobrantes_centro >= 0) {
        centros_estadisticas[centro_vacunacion].disponibles = sobrantes_centro;
    }

    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
}


void imprimirEstadisticaFinal(FILE *salida){
    fprintf(salida, "\nVACUNACION ESTADÍSTICAS FINALES:\n");
    printf("\nVACUNACION ESTADÍSTICAS FINALES:\n");

    // Estadísticas de fábricas
    for (int i = 0; i < fabricas; i++) {
        fprintf(salida, "Fábrica %d: Fabricadas = %d\n", i + 1, fabricas_estadisticas[i].fabricadas);
        printf("Fábrica %d: Fabricadas = %d\n", i + 1, fabricas_estadisticas[i].fabricadas);
        for (int j = 0; j < centros_vacunacion; j++) {
            fprintf(salida, "  -> Entregadas al Centro %d = %d\n", j + 1, fabricas_estadisticas[i].entregadas_a_centros[j]);
            printf("  -> Entregadas al Centro %d = %d\n", j + 1, fabricas_estadisticas[i].entregadas_a_centros[j]);
        }
    }
    printf("\n");
    fprintf(salida,"\n");
    
    // Estadísticas de centros de vacunación
    for (int i = 0; i < centros_vacunacion; i++) {
        fprintf(salida, "Centro %d: Recibidas = %d, Vacunados = %d, Sobrantes = %d.\n", 
                i + 1, centros_estadisticas[i].recibidas, centros_estadisticas[i].vacunados, centros_estadisticas[i].disponibles);
        printf("Centro %d: Recibidas = %d, Vacunados = %d, Sobrantes = %d\n", 
                i + 1, centros_estadisticas[i].recibidas, centros_estadisticas[i].vacunados, centros_estadisticas[i].disponibles);
    }
}

int main(int argc, char *argv[]){

    FILE *fd_Entrada;
    FILE *fd_Salida;
    int contador_habitantes=0;//identificador de los habitantes, empezando por el habitante 1

    pthread_mutex_init(&mutex, NULL); //inicializamos el mutex

    //ENTRADA DE ARGUMENTOS
    if(argc ==1){//se pone los nombres de fichero de entrada y salida por defecto, el de entrada ya existe siempre no hay que crearlo, el de salida sí
        fd_Entrada=fopen("entrada_vacunacion.txt","r");
        if(fd_Entrada == NULL){
            perror("Error al abrir el fichero de entrada.\n");
            return 1;
        }
        
        fd_Salida=fopen("salida_vacunacion.txt","w+");
        
    }else if (argc == 2){
        fd_Entrada=fopen(argv[1],"r");
        if(fd_Entrada == NULL){
            perror("Error al abrir el fichero de entrada.\n");
            return 1;
        }

        fd_Salida=fopen("salida_vacunacion.txt","w+");

    }else if(argc == 3){
        fd_Entrada=fopen(argv[1],"r");
        if(fd_Entrada == NULL){
            perror("Error al abrir el fichero de entrada.\n");
            return 1;
        }

        fd_Salida=fopen(argv[2],"w+");

    }else{
        printf("Error. Uso: %s [entrada_vacunacion.txt] [salida_vacunacion.txt]", argv[0]);
        return 1;
    }

    variables(fd_Entrada);
    imprimirConfiguracionInicial(fd_Salida);

    //Inicialización de los hilos de fábricas
    pthread_t hilos_fabricas[fabricas];
    int id_fabricas[fabricas];
    for(int i = 0;i < fabricas;i++){
        id_fabricas[i] = i; //asignamos un identificador a cada fábrica
        if(pthread_create(&hilos_fabricas[i],NULL,fabrica_vacunas,&id_fabricas[i]) != 0){
            perror("Error al crear el hilo de la fabrica");
            return 0;
        }
        
    }

    // Inicialización de hilos de habitantes
    pthread_t hilos_habitantes[habitantes];
    int id_habitantes[habitantes];
    for (int i = 0; i < 10; i++) {//10 es el numero fijo de tandas, PARAMETRIZAR
        for(int j=0; j< vacunas_tanda; j++){
            id_habitantes[j] = contador_habitantes++;
            pthread_create(&hilos_habitantes[j], NULL, habitante_vacunacion, &id_habitantes[j]);
        }

        for (int j = 0; j < vacunas_tanda; j++) {
            pthread_join(hilos_habitantes[j], NULL);
        }
    }

    //lo ponemos en bucle separado para que primero se creen todos los hilos y luego esperar a cada uno
    for(int i = 0; i < fabricas; i++){
        pthread_join(hilos_fabricas[i],NULL);
    }
    
    
    imprimirEstadisticaFinal(fd_Salida);
    printf("Vacunación finalizada.\n");

    // Liberar memoria dinámica
    for (int i = 0; i < fabricas; i++) {
        free(fabricas_estadisticas[i].entregadas_a_centros);
    }
    for (int i = 0; i < centros_vacunacion; i++) {
        free(centros_estadisticas[i].habitantesVacunados);
         pthread_cond_destroy(&centros_estadisticas[i].cond);
    }

    free(fabricas_estadisticas);
    free(centros_estadisticas);

    pthread_mutex_destroy(&mutex); //destruimos el mutex

    fclose(fd_Entrada);
    fclose(fd_Salida);
    

    return 0;
}