#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>  // Añadir esta línea

#define PIPE_NAME "pipe_comunicacion"
#define MAX_CLIENTS 10

// Estructura para pasar información al hilo
typedef struct {
    int client_id;
    int pipe_fd;
} client_data_t;

// Función que se ejecutará en cada hilo para gestionar la comunicación con un cliente
void *handle_client(void *arg) {
    client_data_t *data = (client_data_t *)arg;
    int client_id = data->client_id;
    int pipe_fd = data->pipe_fd;
    
    // Asignar un token único al cliente
    int token = client_id;
    write(pipe_fd, &token, sizeof(int));
    printf("Servidor asignó token %d al cliente %d\n", token, client_id);
    
    // Aquí puedes agregar la lógica de comunicación con el cliente
    // Ejemplo de envío de un mensaje al cliente (simulado con el pipe)
    char mensaje[256];
    snprintf(mensaje, sizeof(mensaje), "Hola cliente %d, tu token es %d", client_id, token);
    write(pipe_fd, mensaje, strlen(mensaje) + 1);
    
    // Cerrar el pipe después de escribir
    close(pipe_fd);  // Cerrar el descriptor de archivo
    free(arg);        // Liberar memoria de la estructura
    pthread_exit(NULL); // Salir del hilo
}

int main() {
    printf("Servidor iniciado\n");

    // Eliminar el pipe si ya existe
    unlink(PIPE_NAME);  // Esto eliminará el pipe si ya existe.

    // Crear el pipe (FIFO)
    if (mkfifo(PIPE_NAME, 0666) == -1) {
        perror("Error al crear la FIFO");
        exit(1);
    }

    int client_id = 0;
    while (1) {
        // Abrir el pipe para escribir
        int pipe_fd = open(PIPE_NAME, O_WRONLY);
        if (pipe_fd == -1) {
            perror("Error al abrir el pipe");
            continue;
        }

        // Crear un hilo para atender al cliente
        pthread_t thread_id;
        client_data_t *data = malloc(sizeof(client_data_t));
        data->client_id = client_id;
        data->pipe_fd = pipe_fd;

        if (pthread_create(&thread_id, NULL, handle_client, data) != 0) {
            perror("Error al crear el hilo");
            close(pipe_fd);  // Cerrar el descriptor de archivo en caso de error
            free(data);
            continue;
        }
        
        // Incrementar el ID del cliente para el próximo
        client_id++;

        // Esperar que el hilo termine
        pthread_detach(thread_id);  // El hilo se "desprende", por lo que no se espera que termine
    }

    // Eliminar el pipe (FIFO)
    unlink(PIPE_NAME);
    return 0;
}

