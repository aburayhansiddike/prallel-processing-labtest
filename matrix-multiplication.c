#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include<unistd.h>

int main(int argc, char** argv){

    MPI_Init(&argc, &argv);
    
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int NUM_MATRICS, MATRICS_ROWS, MATRICS_COLS, MATRICS_B_COLS;

    double startTime, endTime;
    if(rank == 0){
        printf("Number of matrices : \n");
        scanf("%d", &NUM_MATRICS);
        printf("Number of rows in matrix A : \n");
        scanf("%d", &MATRICS_ROWS);
        printf("Number of colums in matrix A : \n");
        scanf("%d", &MATRICS_COLS);
        printf("Number of colums in matrix B : \n");
        scanf("%d", &MATRICS_B_COLS);
    }

    MPI_Bcast(&NUM_MATRICS, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&MATRICS_ROWS, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&MATRICS_COLS, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&MATRICS_B_COLS, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if(NUM_MATRICS % size != 0){
        printf("Number of matrics must be divisible by the number of process\n");
        MPI_Finalize();
        return 1;
    }

    int root = 0;
    int matrixA[NUM_MATRICS][MATRICS_ROWS][MATRICS_COLS];
    int matrixB[NUM_MATRICS][MATRICS_COLS][MATRICS_B_COLS];

    if(rank == root){
        for(int k = 0; k < NUM_MATRICS; k++){
            for(int i = 0; i < MATRICS_ROWS; i++){
                for(int j = 0; j < MATRICS_COLS; j++){
                    matrixA[k][i][j] = rand() % 10;
                }
            }

            for(int i = 0; i < MATRICS_COLS; i++){
                for(int j = 0; j < MATRICS_B_COLS; j++){
                    matrixB[k][i][j] = rand() % 10;
                }
            }
        }


        for(int k = 0; k < NUM_MATRICS; k++){
            printf("Number of matrix : %d\n", k);
            printf("------------------------------------------------\n");
            printf("Matrix A :\n");
            for(int i = 0; i < MATRICS_ROWS; i++){
                for(int j = 0; j < MATRICS_COLS; j++){
                    printf("%d ", matrixA[k][i][j]);
                }
                printf("\n");
            }

            printf("Matrix B : \n");
            for(int i = 0; i < MATRICS_COLS; i++){
                for(int j = 0; j < MATRICS_B_COLS; j++){
                    printf("%d ", matrixB[k][i][j]);
                }
                printf("\n");
            }
        }
    }

    //printf("rank before :  %d\n", rank);
    MPI_Barrier(MPI_COMM_WORLD);
    //printf("rank after :  %d\n", rank);
    startTime = MPI_Wtime();

    // //distribute the matrix
    int localMatrixA[NUM_MATRICS / size][MATRICS_ROWS][MATRICS_COLS];
    int localMatrixB[NUM_MATRICS / size][MATRICS_COLS][MATRICS_B_COLS];
    int localResultMatrix[NUM_MATRICS / size][MATRICS_ROWS][MATRICS_B_COLS];

    int chunkSize = (NUM_MATRICS / size) * MATRICS_ROWS * MATRICS_COLS;
    MPI_Scatter(matrixA, chunkSize, MPI_INT, localMatrixA, chunkSize, MPI_INT, root, MPI_COMM_WORLD);

    chunkSize = (NUM_MATRICS / size) * MATRICS_COLS * MATRICS_B_COLS;
    MPI_Scatter(matrixB, chunkSize, MPI_INT, localMatrixB, chunkSize, MPI_INT, root, MPI_COMM_WORLD);

    // if(rank == 0){
    //     printf("Rank : %d\n", rank);
    //     for(int k = 0; k < NUM_MATRICS / size; k++){
    //         for(int i = 0; i < MATRICS_ROWS; i++){
    //             for(int j = 0; j < MATRICS_COLS; j++){
    //                 printf("%d ", localMatrixA[k][i][j]);
    //             }
    //             printf("\n");
    //         }

    //         for(int i = 0; i < MATRICS_ROWS; i++){
    //             for(int j = 0; j < MATRICS_B_COLS; j++){
    //                 printf("%d ", localMatrixB[k][i][j]);
    //             }
    //             printf("\n");
    //         }
    //     }
    // }
    //matrix multiplication
    for(int k = 0; k < NUM_MATRICS / size; k++){
        for(int i = 0; i < MATRICS_ROWS; i++){
            for(int j = 0; j < MATRICS_B_COLS; j++){
                localResultMatrix[k][i][j] =  0;
                for(int l = 0; l < MATRICS_COLS; l++){
                    localResultMatrix[k][i][j] += (localMatrixA[k][i][l] * localMatrixB[k][l][j]);
                }
            }
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    endTime = MPI_Wtime();

    printf("Process %d : Time taken = %f seconds\n", rank, endTime - startTime);


    //gather result;
    chunkSize = (NUM_MATRICS / size) * MATRICS_ROWS * MATRICS_B_COLS;
    int gatheredMatrix[NUM_MATRICS][MATRICS_ROWS][MATRICS_B_COLS];
    MPI_Gather(localResultMatrix, chunkSize, MPI_INT, gatheredMatrix, chunkSize, MPI_INT, root, MPI_COMM_WORLD);


    // //printMatrix
    // if(rank == root){
    //     for(int k = 0; k < NUM_MATRICS; k++){
    //         printf("Matrix : %d\n", k);
    //         printf("------------------------------------------\n");
    //         for(int i = 0; i < MATRICS_ROWS; i++){
    //             for(int j = 0; j < MATRICS_COLS; j++){
    //                 printf("%d ", gatheredMatrix[k][i][j]);
    //             }
    //             printf("\n");
    //         }
    //         printf("\n\n");
    //     }
    // }
    MPI_Finalize();
    return 0;
}