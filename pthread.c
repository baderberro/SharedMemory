#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <omp.h>
#include <pthread.h>
#include "matrix.h"


#define DEFAULT_NUM_THREADS 10

struct v {
    int i;
    int j; 
    matrix_struct *m_1;
    matrix_struct *m_2;
    matrix_struct *result_matrix;
};

void *runner(void *param);

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
    for(int i=0; i < result_matrix->rows; ++i)
        result_matrix->mat_data[i]=calloc(result_matrix->cols, sizeof(double));

    double start_time = omp_get_wtime();

    int i,j, count = 0;
    for(i = 0; i < result_matrix->rows; i++) {
        for(j = 0; j < result_matrix->cols; j++) {
            struct v *data = (struct v *) malloc(sizeof(struct v));
            data->i = i;
            data->j = j;
            data->m_1 = m_1;
            data->m_2 = m_2;
            data->result_matrix = result_matrix;
            pthread_t tid;
            pthread_attr_t attr;
            pthread_attr_init(&attr);
            pthread_create(&tid,&attr,runner,data);
            pthread_join(tid, NULL);
            count++;
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

void *runner(void *param) {
    struct v *data = param; 
    int n = 0;
    double sum = 0.0;
    for(n = 0; n < data->m_2->rows; n++){
        sum += data->m_1->mat_data[data->i][n] * data->m_2->mat_data[n][data->j];
    }
    data->result_matrix->mat_data[data->i][data->j] += sum; 
    pthread_exit(0);
}