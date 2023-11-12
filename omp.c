#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <omp.h>
#include "matrix.h"


int main(int argc, char **argv)
{
    if(argc != 3){
        printf("ERROR: Please specify only 2 files.\n");
        exit(EXIT_FAILURE);
    }
        
    matrix_struct *m_1 = get_matrix_struct(argv[1]);
    matrix_struct *m_2 = get_matrix_struct(argv[2]);
    printf("Matrix A:\n");
    print_matrix(m_1);

    printf("Matrix B:\n");
    print_matrix(m_2);
    printf("Matrix A dimensions: %u x %u\n", m_1->rows, m_1->cols);
    printf("Matrix B dimensions: %u x %u\n", m_2->rows, m_2->cols);

    if(m_1->cols != m_2->rows){
        printf("ERROR: The number of columns of matrix A must match the number of rows of matrix B.\n");
        exit(EXIT_FAILURE);
    }

    matrix_struct *result_matrix = malloc(sizeof(matrix_struct));
    result_matrix->rows = m_1->rows;
    result_matrix->cols = m_2->cols;
    result_matrix->mat_data = calloc(result_matrix->rows, sizeof(double*)); 
    for(int i = 0; i < result_matrix->rows; ++i)
        result_matrix->mat_data[i]=calloc(result_matrix->cols, sizeof(double));
    omp_set_num_threads(10);
    double start_time = omp_get_wtime();
    #pragma omp parallel for
    for (int i = 0; i < result_matrix->rows; i++) {
        for (int j = 0; j < result_matrix->cols; j++) {
            for (int k = 0; k < m_1->cols; k++) {
                result_matrix->mat_data[i][j] += m_1->mat_data[i][k] * m_2->mat_data[k][j];
            }
        }
    }
    double end_time = omp_get_wtime();
    double elapsed_time = end_time - start_time;
    printf("Time taken: %f seconds\n", elapsed_time);
    printf("The result matrix: \n");
    print_matrix(result_matrix);
    
    free_matrix(m_1);
    free_matrix(m_2);
    free_matrix(result_matrix);
    
    exit(EXIT_SUCCESS);
}