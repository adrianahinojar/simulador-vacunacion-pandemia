# Simulador-Vacunacion-Pandemia
Simulación de un sistema de vacunación en pandemia utilizando hilos en C. El programa gestiona la producción, distribución y administración de vacunas en centros de vacunación, asegurando la sincronización mediante mutex y variables de condición en POSIX Threads, con el uso de la librería `pthread` en Linux. 


## Descripción

El programa tiene como entrada una serie de parámetros que se leen desde el archivo `entrada.txt`. A partir de esos parámetros, el programa simula la producción de vacunas, su distribución a diferentes centros de salud, y la asignación de citas a los ciudadanos. La simulación se realiza en múltiples hilos, que permiten una ejecución concurrente de las diferentes etapas del proceso de vacunación.

El programa está diseñado para funcionar en un entorno Linux, utilizando la librería `pthread` para gestionar los hilos. 

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
