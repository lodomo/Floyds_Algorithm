#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define OPTIONS "f:hsS"
#define FALSE 0
#define TRUE 1
#define INT_MAX 1000000

void process_options(int argc, char *argv[]);
char *get_data(void);
int **create_matrix(char *data);
int min(int a, int b);
void print_matrix(int **matrix);
void floyds_algorithm(int **matrix);
void copy_matrix(int **input, int **output);
void show_step(int **matrix, int, int, int);

char *i_file = NULL;
int matrix_size = 0;
int show = FALSE;
int **temp = NULL; // For showing each step

int main(int argc, char *argv[]) {
    int **matrix = NULL;
    char *raw_data = NULL;
    process_options(argc, argv);
    raw_data = get_data();
    matrix = create_matrix(raw_data);

    fprintf(stderr, "k = 0: Original Matrix\n\n");
    print_matrix(matrix);

    if (show) {
        fprintf(stderr, "Press Enter to Continue\n");
        getchar();
    }

    floyds_algorithm(matrix);
    fprintf(stderr, "\n\nk = %d: Final Matrix\n\n", matrix_size);
    print_matrix(matrix);

    free(raw_data);
    free(matrix);
    if (temp) {
        free(temp);
    }
    return EXIT_SUCCESS;
}

void process_options(int argc, char *argv[]) {
    // Process command line options
    int opt = 0;

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'f':
            i_file = optarg;
            fprintf(stderr, "Input file: %s\n", i_file);
            break;
        case 's':
            show = TRUE;
            fprintf(stderr, "Show Each K Step\n");
            break;
        case 'S':
            show = 2;
            fprintf(stderr, "Show EVERY SINGE Step\n");
            break;
        case 'h':
            fprintf(stderr, "Usage: [-f input_file]\n");
            break;
        default:
            fprintf(stderr, "Usage: [-f input_file]\n");
            break;
        }
    }

    if (i_file == NULL) {
        fprintf(stderr, "Usage: [-f input_file]\n");
        exit(EXIT_FAILURE);
    }
}

char *get_data(void) {
    int ifd = 0;
    struct stat i_stat;
    char *data = NULL;
    memset(&i_stat, 0, sizeof(i_stat));
    if ((ifd = open(i_file, O_RDONLY)) < 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    if (stat(i_file, &i_stat) < 0) {
        perror("stat");
        exit(EXIT_FAILURE);
    }

    if ((data = (char *)malloc(i_stat.st_size)) == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    if (read(ifd, data, i_stat.st_size) < 0) {
        perror("read");
        exit(EXIT_FAILURE);
    }

    if (close(ifd) < 0) {
        perror("close");
        exit(EXIT_FAILURE);
    }

    return data;
}

int **create_matrix(char *data) {
    int **matrix = NULL;
    char *token = NULL;

    for (size_t i = 0; i < strlen(data); i++) {
        if (data[i] == '\n') {
            matrix_size++;
        }
    }

    // Allocate the rows
    if ((matrix = (int **)malloc(matrix_size * sizeof(int *))) == NULL) {
        perror("malloc");
        return NULL;
    }

    // Allocate the columns
    for (int i = 0; i < matrix_size; i++) {
        if ((matrix[i] = (int *)malloc(matrix_size * sizeof(int))) == NULL) {
            perror("malloc");
            return NULL;
        }
    }

    // STRTOK and atoi to fill the matrix
    token = strtok(data, " \n");

    for (int i = 0; i < matrix_size; i++) {
        for (int j = 0; j < matrix_size; j++) {
            if (token == NULL) {
                break;
            }
            if (token[0] == '~') {
                matrix[i][j] = INT_MAX;
                token = strtok(NULL, " \n");
                continue;
            }
            matrix[i][j] = atoi(token);
            token = strtok(NULL, " \n");
        }
    }

    // Zero the diagnols
    for (int i = 0; i < matrix_size; i++) {
        matrix[i][i] = 0;
    }

    if (show) {
        if ((temp = (int **)malloc(matrix_size * sizeof(int *))) == NULL) {
            perror("malloc");
            return NULL;
        }

        for (int i = 0; i < matrix_size; i++) {
            if ((temp[i] = (int *)malloc(matrix_size * sizeof(int))) == NULL) {
                perror("malloc");
                return NULL;
            }
        }

        copy_matrix(matrix, temp);
    }

    return matrix;
}

int min(int a, int b) { return (a < b) ? a : b; }

void print_matrix(int **matrix) {
    fprintf(stderr, "   ");
    for (int i = 0; i < matrix_size; ++i) {
        fprintf(stderr, "%2c ", 'A' + i);
    }
    fprintf(stderr, "\n");

    // Test print this matrix
    for (int i = 0; i < matrix_size; i++) {
        // Print the row as A, B ... Matrix size
        for (int j = 0; j < matrix_size; j++) {
            if (j == 0) {
                printf("%2c ", 'A' + i);
            }

            if (matrix[i][j] == INT_MAX) {
                printf(" ~ ");
                continue;
            }
            printf("%2d ", matrix[i][j]);
        }
        printf("\n");
    }
    return;
}

void floyds_algorithm(int **matrix) {
    // K is the loop that goes through each "road block"
    for (int k = 0; k < matrix_size; ++k) {
        // i is the row
        for (int i = 0; i < matrix_size; ++i) {
            // j is the column
            for (int j = 0; j < matrix_size; ++j) {
                if (i == j || i == k || j == k) {
                    continue;
                }

                matrix[i][j] = min(matrix[i][j], matrix[i][k] + matrix[k][j]);

                if (show == 2) {
                    show_step(matrix, k + 1, j, i);
                }
            }
        }

        if (show == 1) {
            show_step(matrix, k + 1, -1, -1);
        }
    }
}



void copy_matrix(int **input, int **output) {
    for (int i = 0; i < matrix_size; i++) {
        for (int j = 0; j < matrix_size; j++) {
            output[i][j] = input[i][j];
        }
    }
}

void show_step(int **matrix, int k, int x_j, int x_i) {
    int identical = TRUE;

    for (int i = 0; i < matrix_size; i++) {
        for (int j = 0; j < matrix_size; j++) {
            if (matrix[i][j] != temp[i][j]) {
                identical = FALSE;
                break;
            }
        }
    }

    if (identical) {
        return;
    }

    fprintf(stderr, "\n\nk = %d: Matrix\n\n", k);

    if (show == 2 && x_i != -1 && x_j != -1) {
        fprintf(stderr, "Comparing %c -> %c > %c -> %c -> %c\n", 'A' + x_i,
                'A' + x_j, 'A' + x_i, 'A' + k - 1, 'A' + x_j);
    }

    fprintf(stderr, "   ");
    for (int i = 0; i < matrix_size; ++i) {
        fprintf(stderr, "%2c ", 'A' + i);
    }
    fprintf(stderr, "\n");

    for (int i = 0; i < matrix_size; i++) {
        for (int j = 0; j < matrix_size; j++) {
            if (j == 0) {
                printf("%2c ", 'A' + i);
            }

            if (matrix[i][j] == INT_MAX) {
                printf(" ~ ");
                continue;
            }

            if (matrix[i][j] == temp[i][j]) {
                printf("%2d ", matrix[i][j]);
                continue;
            }

            // Make the text underlined
            printf("\033[4m%2d\033[0m ", matrix[i][j]);
        }
        printf("\n");
    }

    copy_matrix(matrix, temp);

    // Wait for user to press enter
    fprintf(stderr, "Press Enter to Continue\n");
    getchar();
}
