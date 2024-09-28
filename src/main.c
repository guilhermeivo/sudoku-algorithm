#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 9
#define M 9
#define SIZE 1
#define AMOUNT_DIVISIONS 3
#define AMOUNT_START 18

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

int getQuadrantCenter(int n, int m, int u, int v, int* uQuadrantCenter, int* vQuadrantCenter) {
    if (u < 0 || u >= n || v < 0 || v >= m) return 0;

    int uPosition = 0;
    int vPosition = 0;
    for (int i = 0; i <= u; i+=3) {
        uPosition++;
    }
    for (int i = 0; i <= v; i+=3) {
        vPosition++;
    }

    *uQuadrantCenter = (uPosition - 1) * 3 + 1;
    *vQuadrantCenter = (vPosition - 1) * 3 + 1;

    return 1;
}

int isValidPositionWithPositions(int *board, int n, int m, int u, int v, int value, int *uInvalid, int *vInvalid) {
    if (u < 0 || u >= m || v < 0 || v >= n) return 0;

    int uQuadrantCenter; 
    int vQuadrantCenter;
    int success = getQuadrantCenter(n, m, u, v, &uQuadrantCenter, &vQuadrantCenter);
    if (!success) return 0;

    int uIndex[9] = { -1, -1, -1, 0, 0, 0, 1, 1, 1 };
    int vIndex[9] = { -1, 0, 1, -1, 0, 1, -1, 0, 1 };

    for (int i = 0; i < 9; i++) {
        int uCurrent = uIndex[i] + uQuadrantCenter;
        int vCurrent = vIndex[i] + vQuadrantCenter;

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

int isValidPosition(int *board, int n, int m, int u, int v, int value) {
    int uInvalid, vInvalid;
    return isValidPositionWithPositions(board, n, m, u, v, value, &uInvalid, &vInvalid);
}

int isComplete(int *board, int n, int m) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (board[i * m + j] < 0) return 0;
            int valid = isValidPosition(board, n, m, i, j, board[i * m + j]);
            if (!valid) return 0;
        }
    }

    return 1;
}

void initBoard(int *board, int n, int m, int amountValuesStart) {
    for (int i = 0; i < n * m; i++) {
        board[i] = -1;
    }
    srand(time(NULL));
    int counter = 0;
    while (counter < amountValuesStart || isComplete(board, n, m)) {
        int value = (random() % 9) + 1;
        int index = random() % (n * m);
        int aux = index;
        int v = 0;
        while (aux >= m) {
            aux -= m;
        }
        v = aux;
        int u = (index - v) / n;
        if (isValidPosition(board, n, m, u, v, value)) {
            board[index] = value;
        }
        else counter--;

        counter++;
    }
}

void printBoard(int *board, int *defaultBoard, int n, int m, int u, int v) {
    int uInvalid = -1;
    int vInvalid = -1;
    int valid = 1;
    if (u >= 0 && v >= 0) {
        valid = isValidPositionWithPositions(board, n, m, u, v, board[u * m + v], &uInvalid, &vInvalid);
    }
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            int value = board[i * m + j];
            if (value < 0) {
                printf("\033[90m . \033[m");
            } else {
                if (defaultBoard[i * m + j] > 0) {
                    if ((uInvalid == i && vInvalid == j)) {
                        printf("\033[91m %0*d \033[m", SIZE, board[i * m + j]); // invalid
                    } else {
                        printf("\033[90m %0*d \033[m", SIZE, board[i * m + j]); // default
                    }
                } else {
                    if (!valid && (uInvalid == i && vInvalid == j)) {
                        printf("\033[91m %0*d \033[m", SIZE, board[i * m + j]); // invalid
                    } else {
                        if (i == u && v == j) {
                            printf("\033[93m %0*d \033[m", SIZE, board[i * m + j]); // in progress
                        } else {
                            printf("\033[94m %0*d \033[m", SIZE, board[i * m + j]); // possible
                        }
                    }
                }
            }
            
            if (j % (m / AMOUNT_DIVISIONS) == AMOUNT_DIVISIONS - 1 && j != m - 1) {
                printf("\033(0");
                printf("%c", 0x78);
                printf("\033(B");
            }
        }
        
        printf("\n");
        if (i % 3 == 2 && i != n - 1) {
            printf("\033(0");
            for (int j = 0; j < m; j++) {
                if (j % (m / AMOUNT_DIVISIONS) == AMOUNT_DIVISIONS - 1 && j != m - 1) {
                    for (int k = 0; k < SIZE + 2; k++) {
                        printf("%c", 0x71);
                    }
                    printf("%c", 0x6E);
                } else {
                    for (int k = 0; k < SIZE + 2; k++) {
                        printf("%c", 0x71);
                    }
                }
            }
            printf("\033(B");
            printf("\n");
        }
    }
}

void sudoku(int *board, int n, int m, int x, int y, int *defaultBoard) {
    int u = y;
    int v = x;
    int k = 1; // 1-9

    do {
        if (u * m + v >= 0 && u * m + v < n * m) {
            if (board[u * m + v] < 0) {
                #ifdef DEBUG
                board[u * m + v] = k;
                printBoard(board, defaultBoard, n, m, u, v);
                nanosleep((const struct timespec[]){{0, 500000000L}}, NULL);
                printf("\033[%dA", N + AMOUNT_DIVISIONS - 1);
                printf("\033[0J");
                board[u * m + v] = -1;
                #endif

                int valid = isValidPosition(board, n, m, u, v, k);
                if (valid) {
                    board[u * m + v] = k;
                    int nextX = x + 1;
                    int nextY = y;
                    if (nextX >= m) {
                        nextX = 0;
                        nextY++;
                    }
                    sudoku(board, n, m, nextX, nextY, defaultBoard);

                    if (!isComplete(board, n, m)) board[u * m + v] = -1;
                }
            } else {
                int nextX = x + 1;
                int nextY = y;
                if (nextX >= m) {
                    nextX = 0;
                    nextY++;
                }
                sudoku(board, n, m, nextX, nextY, defaultBoard);

                if (!isComplete(board, n, m)) return;
            }
        }
        k++;
    } while(!(isComplete(board, n, m) || k > 9));
}

int main(int argc,char *argv[]) {
    double start, stop;
    int *board, *defaultBoard;
    int n, m;
    n = N;
    m = M;
    board = (int*) malloc(n * m * sizeof(int));
    defaultBoard = (int*) malloc(n * m * sizeof(int));
    if (board == NULL || defaultBoard == NULL) {
        printf("Erro ao manipular o armazenamento.");
        return 1;
    }
    if (argc > 1) {
        FILE *fptr;
        fptr = fopen(argv[1], "r");
        if (fptr == NULL) {
            perror("Erro ao abrir o arquivo");
            return 1;
        }
        for (int i = 0; i < n * m; i++) {
            fscanf(fptr, "%d", &board[i]);
        }
        fclose(fptr);
    } else {
        initBoard(board, n, m, AMOUNT_START);
    }
    for (int i = 0; i < n * m; i++) {
        defaultBoard[i] = board[i];
    }
    
    printf("\033[5m=====\033[m \033[1mSudoku's solution\033[m \033[5m=====\033[m\n\n");
    start = time_as_double();
    sudoku(board, n, m, 0, 0, defaultBoard);
    stop = time_as_double();
    printBoard(board, defaultBoard, n, m, -1, -1);
    if (!isComplete(board, n, m)) printf("Infelizmente, esta composicao de numeros nao tem solucao.\n");
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