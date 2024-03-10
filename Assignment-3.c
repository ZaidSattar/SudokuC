#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define SIZE 9 //  3x3 grids
#define NUM_THREADS 27 // 27 threads

typedef struct {
    int row;
    int col;
    int (*sudoku)[SIZE];
} parameters;

void *parseRow(void *param) { // this function will parse through each row
    parameters *p = (parameters *)param;
    int row = p->row;
    int valid[SIZE] = {0};

    for (int i = 0; i < SIZE; i++) { // this loop will check for invalid thread

        int num = p->sudoku[row][i];
        if (num < 1) { // checks if num is less than 1
            printf("Thread # %d (row %d) is INVALID\n", row + 10, row + 1);
            pthread_exit((void *)0);
        } else if (num > 9) { // or if it more than 9
            printf("Thread # %d (row %d) is INVALID\n", row + 10, row + 1);
            pthread_exit((void *)0);
        } else if (valid[num - 1] == 1) {
            printf("Thread # %d (row %d) is INVALID\n", row + 10, row + 1);
            pthread_exit((void *)0);
        }
        valid[num - 1] = 1;
    }

    printf("Thread # %d (row %d) is valid\n", row + 10, row + 1);
    pthread_exit((void *)1);
}

void *parseColumn(void *param) { // this function will be in charge of parsing through the columns
    parameters *p = (parameters *) param;
    int col = p->col;
    int valid[SIZE] = {0};

    for (int i = 0; i < SIZE; i++) { // this loop is intended to check for invalid

        int num = p->sudoku[i][col];
        if (num < 1) { // checks if number is less than 1
            printf("Thread # %d (column %d) is INVALID\n", col + 19, col + 1);
            pthread_exit((void *) 0);
        } else if (num > 9) { //checks if num is more than 9
            printf("Thread # %d (column %d) is INVALID\n", col + 19, col + 1);
            pthread_exit((void *) 0);
        } else if (valid[num - 1] == 1) {
            printf("Thread # %d (column %d) is INVALID\n", col + 19, col + 1);
            pthread_exit((void *) 0);
        } else {
            valid[num - 1] = 1;
        }

        printf("Thread # %d (column %d) is valid\n", col + 19, col + 1); // this will print if it is valid
        pthread_exit((void *) 1);
    }
}

void *parseSub(void *param) { // this function will parse through the sub grids
    parameters *p = (parameters *)param;
    int rowStart = p->row;
    int colStart = p->col;
    int valid[SIZE] = {0};

    for (int i = rowStart; i < rowStart + 3; i++) { // for loop will check for invalid threads
        for (int j = colStart; j < colStart + 3; j++) {
            int num = p->sudoku[i][j];
            if (num < 1) {
                printf("Thread # %d (subgrid %d) is INVALID\n", 1 + (rowStart / 3) * 3 + (colStart / 3), 1 + (rowStart / 3) * 3 + (colStart / 3));
                pthread_exit((void *)0);
            } else if (num > 9) {
                printf("Thread # %d (subgrid %d) is INVALID\n", 1 + (rowStart / 3) * 3 + (colStart / 3), 1 + (rowStart / 3) * 3 + (colStart / 3));
                pthread_exit((void *)0);
            } else if (valid[num - 1] == 1) {
                printf("Thread # %d (subgrid %d) is INVALID\n", 1 + (rowStart / 3) * 3 + (colStart / 3), 1 + (rowStart / 3) * 3 + (colStart / 3));
                pthread_exit((void *)0);
            }
            valid[num - 1] = 1;
        }
    }
    printf("Thread # %d (subgrid %d) is valid\n", 1 + (rowStart / 3) * 3 + (colStart / 3), 1 + (rowStart / 3) * 3 + (colStart / 3)); // any thread that doesnt fall into the invalid section will be put into valid
    pthread_exit((void *)1);

}


int main(int argc, char *argv[]) { // main function
    if (argc != 2) { // checks for incorrect amount of arguments
        printf("Usage: %s <sudoku-puzzle-file>\n", argv[0]);
        return 1;
    }

    int sudoku[SIZE][SIZE];
    FILE *file = fopen(argv[1], "r");
    if (!file) { // simple check to see if the file is having trouble opening
        perror("opening file error");
        return 1;
    }

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (fscanf(file, "%d", &sudoku[i][j]) != 1) {
                fclose(file);
                printf("reading error\n"); // will let the user know if the file is having a reading error
                return 1;
            }
        }
    }
    fclose(file); // closes file when done

    pthread_t threads[NUM_THREADS];
    int threadID = 0;
    parameters param[NUM_THREADS];

    for (int i = 0; i < SIZE; i++, threadID++) { // loop to check rows
        param[threadID].row = i;
        param[threadID].sudoku = sudoku;
        pthread_create(&threads[threadID], NULL, parseRow, &param[threadID]);
    }

    for (int i = 0; i < SIZE; i++, threadID++) { // loop to check columns
        param[threadID].col = i;
        param[threadID].sudoku = sudoku;
        pthread_create(&threads[threadID], NULL, parseColumn, &param[threadID]);
    }

    // Check 3x3 subgrids
    for (int i = 0; i < SIZE; i += 3) {
        for (int j = 0; j < SIZE; j += 3, threadID++) {
            param[threadID].row = i;
            param[threadID].col = j;
            param[threadID].sudoku = sudoku;
            pthread_create(&threads[threadID], NULL, parseSub, &param[threadID]);
        }
    }

    int valid = 1;
    for (int i = 0; i < NUM_THREADS; i++) {
        void *status;
        pthread_join(threads[i], &status);
        if (status == (void *)0) {
            valid = 0;
        }
    }

    if (valid) {
        printf("%s contains a valid solution\n", argv[1]); // at the end checks if valid solution exists
    } else {
        printf("%s contains an INVALID solution\n", argv[1]); ; // at the end checks if invalid solution exists
    }

    return 0;
}
