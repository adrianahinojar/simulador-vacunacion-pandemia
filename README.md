# Simulador-Vacunacion-Pandemia
Simulación de un sistema de vacunación en pandemia utilizando hilos en C. El programa gestiona la producción, distribución y administración de vacunas en centros de vacunación, asegurando la sincronización mediante mutex y variables de condición en POSIX Threads, con el uso de la librería `pthread` en Linux. 


## Descripción

El programa tiene como entrada una serie de parámetros que se leen desde el archivo `entrada.txt`. A partir de esos parámetros, el programa simula la producción de vacunas, su distribución a diferentes centros de salud, y la asignación de citas a los ciudadanos. 

La simulación se realiza en múltiples hilos, que permiten una ejecución concurrente de las diferentes etapas del proceso de vacunación.

Está diseñado para funcionar en un entorno Linux, utilizando la librería `pthread` para gestionar los hilos. 

### Compilación

Para compilar el código fuente `vacunacion.c` en Linux, sigue los siguientes pasos:

1. Abre una terminal.
2. Navega hasta el directorio donde se encuentra el archivo `vacunacion.c`.
3. Ejecuta el siguiente comando de compilación:

   ```bash
   gcc -o vacunacion vacunacion.c -lpthread
  

Esto generará un archivo ejecutable llamado vacunacion que puedes ejecutar en tu sistema.

### Ejecución

Una vez compilado el código, puedes ejecutar el programa utilizando el siguiente comando:

   ```bash
   ./vacunacion
   ```


## Archivos del Proyecto

### 1. vacunacion.c: Código fuente principal del programa.
El archivo `vacunacion.c` contiene la implementación de la simulación del proceso de vacunación. El código está estructurado para:

- Leer los parámetros de entrada desde el archivo `entrada.txt`.
- Crear hilos para simular la fabricación de vacunas, el reparto de vacunas y la asignación de citas.
- Realizar la simulación utilizando los datos obtenidos de los archivos de entrada.
- Generar resultados que se almacenarán en el archivo `salida.txt`.

### 2. entrada.txt: Archivo con datos de entrada.
El archivo `entrada.txt` contiene los parámetros necesarios para la simulación. Los valores que aparecen corresponden a:

1. `habitantes`: El número total de habitantes en la simulación. Este valor define cuántas personas participarán en el proceso de vacunación.

2. `vacunas_iniciales`: El número de vacunas disponibles al principio de la simulación, antes de que comience la producción en las fábricas. Este valor indica las vacunas ya distribuidas a los centros de vacunación.

3. `min_vacunas_tanda`: El número mínimo de vacunas que se fabrican en cada tanda. Este valor establece el rango de producción, lo que significa que no todas las tandas tendrán el mismo número exacto de vacunas.

4. `max_vacunas_tanda`: El número máximo de vacunas que se fabrican por tanda. Al igual que el valor anterior, esto define el límite superior para la cantidad de vacunas que pueden fabricarse por ciclo.

5. `t_min_fabricacion_tanda`: El tiempo mínimo que se tarda en fabricar una tanda de vacunas (en segundos). Este valor afecta a la rapidez con la que las fábricas producen las vacunas y al ritmo con el que se distribuyen.

6. `t_max_fabricacion_tanda`:  
   El tiempo máximo para fabricar una tanda de vacunas (en segundos). Similar al valor anterior, pero permite un rango superior para la variabilidad en el tiempo de fabricación de cada tanda.

7. `t_max_reparto`:  
   El tiempo máximo que se tarda en repartir las vacunas a los centros de vacunación. Este valor afecta la rapidez con la que se distribuyen las vacunas entre los centros, y puede variar dependiendo de la logística simulada en el proceso de reparto.

8. `t_max_cita`:  
   El tiempo máximo para asignar una cita a un habitante. Este valor define la rapidez con la que los ciudadanos pueden recibir su cita para ser vacunados.

9. `t_max_desplazamiento`:  
   El tiempo máximo que tarda un habitante en desplazarse hacia un centro de vacunación. Este parámetro también afecta al tiempo total de simulación, ya que el desplazamiento de los ciudadanos es una parte del proceso de vacunación.
