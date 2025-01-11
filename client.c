#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <ncurses.h>

#define PIPE_NAME "pipe_comunicacion"

int main() {
    // Inicializar ncurses
    initscr();
    cbreak();
    noecho();
    curs_set(0);  // Ocultar el cursor

    // Mostrar una ventana de bienvenida
    printw("Cliente conectado...\n");
    refresh();

    // Abrir el pipe
    int pipe_fd = open(PIPE_NAME, O_RDONLY);
    if (pipe_fd == -1) {
        perror("Error al abrir el pipe");
        endwin(); // Terminar ncurses
        exit(1);
    }

    // Leer el token del servidor
    int token;
    read(pipe_fd, &token, sizeof(int));
    
    // Mostrar el token
    printw("\nCliente recibió token: %d\n", token);

    // Leer el mensaje del servidor
    char mensaje[256];
    read(pipe_fd, mensaje, sizeof(mensaje));
    
    // Mostrar el mensaje del servidor
    printw("Mensaje del servidor: %s\n", mensaje);

    // Esperar a que el usuario presione una tecla para salir
    printw("\nPresione cualquier tecla para salir...");
    refresh();
    getch();  // Espera la entrada del usuario

    // Cerrar el pipe y terminar ncurses
    close(pipe_fd);
    endwin();

    return 0;
}

