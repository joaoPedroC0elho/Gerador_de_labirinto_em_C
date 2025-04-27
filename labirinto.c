#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ROWS 100
#define COLS 100

typedef struct {
    int x, y;
} Ponto;

typedef struct {
    Ponto pontos[ROWS * COLS];
    int inicio, fim;
} Fila;

void readGridFromFile(char *matrix, int rows, int cols, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Erro ao abrir o arquivo");
        return;
    }

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            char ch = fgetc(file);
            if (ch == '\n') ch = fgetc(file);
            *(matrix + r * cols + c) = ch;
        }
    }

    fclose(file);
}

void writeGridToFile(char *matrix, int rows, int cols, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Erro ao abrir o arquivo para escrita");
        return;
    }

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            fputc(*(matrix + r * cols + c), file);
        }
        fputc('\n', file);
    }

    fclose(file);
}

void initializeGrid(char *matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            *(matrix + i * cols + j) = 'x';
}

int isValid(int row, int col, int rows, int cols) {
    return row >= 0 && row < rows && col >= 0 && col < cols;
}

void digMaze(char *matrix, int row, int col, int rows, int cols) {
    *(matrix + row * cols + col) = ' ';

    int directions[4][2] = {{0,1}, {0,-1}, {1,0}, {-1,0}};
    int order[4] = {0,1,2,3};

    for (int i = 0; i < 4; i++) {
        int j = rand() % 4;
        int temp = order[i];
        order[i] = order[j];
        order[j] = temp;
    }

    for (int i = 0; i < 4; i++) {
        int newRow = row + 2 * directions[order[i]][0];
        int newCol = col + 2 * directions[order[i]][1];

        if (isValid(newRow, newCol, rows, cols) && *(matrix + newRow * cols + newCol) == 'x') {
            *(matrix + (row + directions[order[i]][0]) * cols + (col + directions[order[i]][1])) = ' ';
            digMaze(matrix, newRow, newCol, rows, cols);
        }
    }
}

void initializeQueue(Fila *queue) {
    queue->inicio = 0;
    queue->fim = 0;
}

int isQueueEmpty(Fila *queue) {
    return queue->inicio == queue->fim;
}

void enqueue(Fila *queue, Ponto p) {
    queue->pontos[queue->fim++] = p;
}

Ponto dequeue(Fila *queue) {
    return queue->pontos[queue->inicio++];
}

int solveMaze(char *matrix, int rows, int cols) {
    Fila queue;
    initializeQueue(&queue);

    int visited[ROWS][COLS] = {0};
    Ponto parent[ROWS][COLS];

    int startX, startY, endX, endY;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (*(matrix + i * cols + j) == 'A') {
                startX = i;
                startY = j;
            }
            if (*(matrix + i * cols + j) == 'B') {
                endX = i;
                endY = j;
            }
        }
    }

    enqueue(&queue, (Ponto){startX, startY});
    visited[startX][startY] = 1;

    int directions[4][2] = {{0,1},{0,-1},{1,0},{-1,0}};

    while (!isQueueEmpty(&queue)) {
        Ponto current = dequeue(&queue);

        if (current.x == endX && current.y == endY) {
            Ponto p = parent[endX][endY];
            while (!(p.x == startX && p.y == startY)) {
                if (*(matrix + p.x * cols + p.y) == ' ')
                    *(matrix + p.x * cols + p.y) = 'o';
                p = parent[p.x][p.y];
            }
            return 1;
        }

        for (int d = 0; d < 4; d++) {
            int nx = current.x + directions[d][0];
            int ny = current.y + directions[d][1];

            if (isValid(nx, ny, rows, cols) && !visited[nx][ny] && (*(matrix + nx * cols + ny) == ' ' || *(matrix + nx * cols + ny) == 'B')) {
                visited[nx][ny] = 1;
                parent[nx][ny] = current;
                enqueue(&queue, (Ponto){nx, ny});
            }
        }
    }
    return 0;
}

int main(void) {
    srand(time(NULL));
    char grid[ROWS][COLS];
    int option;

    do {
        printf("\n=== Menu ===\n");
        printf("1 - Gerar Grid Aleatorio\n");
        printf("2 - Resolver Caminho\n");
        printf("3 - Sair\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &option);

        if (option == 1) {
            initializeGrid(&grid[0][0], ROWS, COLS);
            digMaze(&grid[0][0], 1, 1, ROWS, COLS);
            grid[1][1] = 'A';
            grid[ROWS-2][COLS-2] = 'B';
            writeGridToFile(&grid[0][0], ROWS, COLS, "grid.txt");
            printf("Grid aleatorio gerado e salvo em grid.txt!\n");
        }
        else if (option == 2) {
            readGridFromFile(&grid[0][0], ROWS, COLS, "grid.txt");
            if (solveMaze(&grid[0][0], ROWS, COLS)) {
                writeGridToFile(&grid[0][0], ROWS, COLS, "path.txt");
                printf("Caminho encontrado e salvo em path.txt!\n");
            } else {
                printf("Nao foi possivel encontrar um caminho.\n");
            }
        }
        else if (option == 3) {
            printf("Saindo...\n");
        }
        else {
            printf("Opçao invalida.\n");
        }

    } while (option != 3);

    return 0;
}
