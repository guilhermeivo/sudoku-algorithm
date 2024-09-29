#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N_HEIGHT 3
#define M_WIDTH 3
#define QUADRANT_DIMENSIONS 3

#define AMOUNT_START 9

#define CLOCK_ID CLOCK_MONOTONIC

static inline double time_as_double(void)
{
	double restime;
	struct timespec ts = { 0 };

	clock_gettime(CLOCK_ID, &ts);

	restime = (double)ts.tv_sec;
	restime += (double)ts.tv_nsec * 1.0e-9;

	return restime;
}

int getQuadrantCenter(const int n, const int m, const int quadrantDimensions, int u, int v, int* uQuadrantCenter, int* vQuadrantCenter) {
    if (u < 0 || u >= n || v < 0 || v >= m) return 0;

    int uPosition = 0;
    int vPosition = 0;
    for (int i = 0; i <= u; i+=quadrantDimensions) {
        uPosition++;
    }
    for (int i = 0; i <= v; i+=quadrantDimensions) {
        vPosition++;
    }

    *uQuadrantCenter = (uPosition - 1) * quadrantDimensions + 1;
    *vQuadrantCenter = (vPosition - 1) * quadrantDimensions + 1;

    return 1;
}

int isValidPositionWithPositions(int *board, const int n, const int m, const int quadrantDimensions, int u, int v, int value, int *uInvalid, int *vInvalid) {
    if (u < 0 || u >= m || v < 0 || v >= n) return 0;

    int uQuadrantCenter; 
    int vQuadrantCenter;
    int success = getQuadrantCenter(n, m, quadrantDimensions, u, v, &uQuadrantCenter, &vQuadrantCenter);
    if (!success) return 0;

    int indexMin = 0;
    indexMin -= QUADRANT_DIMENSIONS / 2;
    int indexMax = QUADRANT_DIMENSIONS / 2;
    if (QUADRANT_DIMENSIONS % 2 == 1) indexMax += 1;
    int uIndex = indexMin; //uIndex[9] = { -1, -1, -1, 0, 0, 0, 1, 1, 1 };
    int vIndex = indexMin; //vIndex[9] = { -1, 0, 1, -1, 0, 1, -1, 0, 1 };
    for (int i = 0; i < QUADRANT_DIMENSIONS * QUADRANT_DIMENSIONS; i++) {
        //uIndex
        int uCurrent = uIndex + uQuadrantCenter;

        if (i % QUADRANT_DIMENSIONS == QUADRANT_DIMENSIONS - 1) {
            uIndex++;
        }
        if (uIndex >= indexMax) uIndex = indexMin;

        //vIndex
        int vCurrent = vIndex + vQuadrantCenter;

        vIndex++;
        if (vIndex >= indexMax) vIndex = indexMin;

        if (uCurrent == u && vCurrent == v) continue;
        if (board[uCurrent * m + vCurrent] >= 0 && value == board[uCurrent * m + vCurrent]) {
            *uInvalid = uCurrent;
            *vInvalid = vCurrent;
            return 0;
        }
    }

    for (int i = 0; i < n; i++) {
        if (u == i) continue;
        if (board[i * m + v] >= 0 && value == board[i * m + v]) {
            *uInvalid = i;
            *vInvalid = v;
            return 0;
        }
    }

    for (int i = 0; i < m; i++) {
        if (v == i) continue;
        if (board[u * m + i] >= 0 && value == board[u * m + i]) {
            *uInvalid = u;
            *vInvalid = i;
            return 0;
        }
    }

    return 1;
}

int isValidPosition(int *board, const int n, const int m, const int quadrantDimensions, int u, int v, int value) {
    int uInvalid, vInvalid;
    return isValidPositionWithPositions(board, n, m, quadrantDimensions, u, v, value, &uInvalid, &vInvalid);
}

int isComplete(int *board, const int n, const int m, const int quadrantDimensions) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (board[i * m + j] < 0) return 0;
            int valid = isValidPosition(board, n, m, quadrantDimensions, i, j, board[i * m + j]);
            if (!valid) return 0;
        }
    }

    return 1;
}

int initBoard(int **board, const int n, const int m, const int amountValuesStart, const int quadrantDimensions) {
    (*board) = (int*) malloc(n * m * sizeof(int));

    if ((*board) == NULL) {
        printf("Erro ao manipular o armazenamento.");
        return 0;
    }
    for (int i = 0; i < n * m; i++) {
        (*board)[i] = -1;
    }
    srand(time(NULL));
    int counter = 0;
    while (counter < amountValuesStart || isComplete((*board), n, m, quadrantDimensions)) {
        int value = (random() % (quadrantDimensions * quadrantDimensions)) + 1;
        int index = random() % (n * m);
        int aux = index;
        int v = 0;
        while (aux >= m) {
            aux -= m;
        }
        v = aux;
        int u = (index - v) / n;
        if (isValidPosition((*board), n, m, quadrantDimensions, u, v, value)) {
            (*board)[index] = value;
        }
        else counter--;

        counter++;
    }
    return 1;
}

void printBoard(int *board, int *defaultBoard, const int n, const int m, const int quadrantDimensions, int u, int v) {
    int uInvalid = -1;
    int vInvalid = -1;
    int valid = 1;
    if (u >= 0 && v >= 0) {
        valid = isValidPositionWithPositions(board, n, m, quadrantDimensions, u, v, board[u * m + v], &uInvalid, &vInvalid);
    }

    int aux = quadrantDimensions * quadrantDimensions;
    int sizeNumber = 0;
    while (aux > 0) {
        aux /= 10;
        sizeNumber++;
    }
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            int value = board[i * m + j];
            if (value < 0) {
                for (int k = 0; k < sizeNumber - 1; k++) {
                    printf(" ");
                }
                printf("\033[90m . \033[m");
            } else {
                if (defaultBoard[i * m + j] > 0) {
                    if ((uInvalid == i && vInvalid == j)) {
                        printf("\033[91m %*d \033[m", sizeNumber, board[i * m + j]); // invalid
                    } else {
                        printf("\033[90m %*d \033[m", sizeNumber, board[i * m + j]); // default
                    }
                } else {
                    if (!valid && (uInvalid == i && vInvalid == j)) {
                        printf("\033[91m %*d \033[m", sizeNumber, board[i * m + j]); // invalid
                    } else {
                        if (i == u && v == j) {
                            printf("\033[93m %*d \033[m", sizeNumber, board[i * m + j]); // in progress
                        } else {
                            printf("\033[94m %*d \033[m", sizeNumber, board[i * m + j]); // possible
                        }
                    }
                }
            }
            
            if (j % quadrantDimensions == quadrantDimensions - 1 && j != m - 1) {
                printf("\033(0");
                printf("%c", 0x78);
                printf("\033(B");
            }
        }
        
        printf("\n");
        if (i % quadrantDimensions == quadrantDimensions - 1 && i != n - 1) {
            printf("\033(0");
            for (int j = 0; j < m; j++) {
                if (j % quadrantDimensions == quadrantDimensions - 1 && j != m - 1) {
                    for (int k = 0; k < sizeNumber + 2; k++) {
                        printf("%c", 0x71);
                    }
                    printf("%c", 0x6E);
                } else {
                    for (int k = 0; k < sizeNumber + 2; k++) {
                        printf("%c", 0x71);
                    }
                }
            }
            printf("\033(B");
            printf("\n");
        }
    }
}

void nextPosition(int x, int y, int m, int *nextX, int *nextY) {
    (*nextX) = x + 1;
    (*nextY) = y;
    if ((*nextX) >= m) {
        (*nextX) = 0;
        (*nextY)++;
    }
}

void sudoku(int *board, const int n, const int m, const int quadrantDimensions, int x, int y, int *defaultBoard) {
    int u = y;
    int v = x;
    int k = 1; // 1-(quadrantDimensions*quadrantDimensions)

    do {
        if (u * m + v >= 0 && u * m + v < n * m) {
            if (board[u * m + v] < 0) {
                #ifdef DEBUG
                board[u * m + v] = k;
                printBoard(board, defaultBoard, n, m, quadrantDimensions, u, v);
                nanosleep((const struct timespec[]){{0, 500000000L}}, NULL);
                printf("\033[%dA", n + (n / QUADRANT_DIMENSIONS) - 1);
                printf("\033[0J");
                board[u * m + v] = -1;
                #endif

                int valid = isValidPosition(board, n, m, quadrantDimensions, u, v, k);
                if (valid) {
                    board[u * m + v] = k;
                    int nextX, nextY;
                    nextPosition(x, y, m, &nextX, &nextY);
                    sudoku(board, n, m, quadrantDimensions, nextX, nextY, defaultBoard);

                    if (!isComplete(board, n, m, quadrantDimensions)) board[u * m + v] = -1;
                }
            } else {
                int nextX, nextY;
                nextPosition(x, y, m, &nextX, &nextY);
                sudoku(board, n, m, quadrantDimensions, nextX, nextY, defaultBoard);

                if (!isComplete(board, n, m, quadrantDimensions)) return;
            }
        }
        k++;
    } while(!(isComplete(board, n, m, quadrantDimensions) || k > quadrantDimensions * quadrantDimensions));
}

int main(int argc,char *argv[]) {
    double start, stop;
    int *board, *defaultBoard;
    int n, m, quadrant_dimensions;
    quadrant_dimensions = QUADRANT_DIMENSIONS;

    if (argc > 1) {
        FILE *fptr;
        fptr = fopen(argv[1], "r");
        if (fptr == NULL) {
            perror("Erro ao abrir o arquivo");
            return 1;
        }
        fscanf(fptr, "%d", &n);
        fscanf(fptr, "%d", &m);
        fscanf(fptr, "%d", &quadrant_dimensions);

        int result = initBoard(&board, n, m, AMOUNT_START, quadrant_dimensions);
        if (!result) return 1;

        for (int i = 0; i < n * m; i++) {
            fscanf(fptr, "%d", &board[i]);
        }
        fclose(fptr);
    } else {
        n = N_HEIGHT * quadrant_dimensions;
        m = M_WIDTH * quadrant_dimensions;

        int result = initBoard(&board, n, m, AMOUNT_START, quadrant_dimensions);
        if (!result) return 1;
    }
    defaultBoard = (int*) malloc(n * m * sizeof(int));
    if (defaultBoard == NULL) {
        printf("Erro ao manipular o armazenamento.");
        return 1;
    }
    for (int i = 0; i < n * m; i++) {
        defaultBoard[i] = board[i];
    }
    
    printf("\033[5m=====\033[m \033[1mSudoku's solution\033[m \033[5m=====\033[m\n\n");
    start = time_as_double();
    sudoku(board, n, m, quadrant_dimensions, 0, 0 ,defaultBoard);
    stop = time_as_double();
    printBoard(board, defaultBoard, n, m, quadrant_dimensions, -1, -1);
    if (!isComplete(board, n, m, quadrant_dimensions)) printf("Infelizmente, esta composicao de numeros nao tem solucao.\n");
    printf("Tempo de execucao: \033[4m%5.5f\033[m segundos.\n", (stop - start));

    if (board != NULL) {
        free(board);
        board = NULL;
    }
    if (defaultBoard != NULL) {
        free(defaultBoard);
        defaultBoard = NULL;
    }

    return 0;
}