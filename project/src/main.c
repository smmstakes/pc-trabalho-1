#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "pool.h"
#include "worker.h"

#define NUM_READERS 11
#define NUM_WRITERS 13

int main()
{
    printf("=== Informacoes do Pool de Conexoes do Banco de Dados === \n");
    printf("Quantidade Maxima de Conexoes Simultaneas: %d\n", MAX_POOL_SIMULTANEOUS_CONNECTIONS);
    printf("Quantidade Maxima de Requisicoes no Pool: %d\n", MAX_POOL_REQUESTS);

    srand((uint8_t)time(NULL));

    connection_pool_t global_pool;
    pool_init(&global_pool);

    pthread_t reader_threads[NUM_READERS];
    pthread_t writer_threads[NUM_WRITERS];

    for (uint8_t i = 0; i < NUM_READERS; i++)
    {
        worker_data_t *data = malloc(sizeof(worker_data_t));

        // 100+ identify the readers
        data->thread_id = i + 101;
        data->pool = &global_pool;

        pthread_create(&reader_threads[i], NULL, bi_reader_worker, data);
    }

    for (uint8_t i = 0; i < NUM_WRITERS; i++)
    {
        worker_data_t *data = malloc(sizeof(worker_data_t));

        // 200+ identify the readers
        data->thread_id = i + 201;
        data->pool = &global_pool;

        pthread_create(&writer_threads[i], NULL, etl_writer_worker, data);
    }

    for (uint8_t i = 0; i < NUM_READERS; i++)
    {
        pthread_join(reader_threads[i], NULL);
    }

    for (uint8_t i = 0; i < NUM_WRITERS; i++)
    {
        pthread_join(writer_threads[i], NULL);
    }

    pool_destroy(&global_pool);

    printf("=== Informacoes Finais do Pool de Conexoes do Banco de Dados === \n");
    printf("Total de Conexões Executadas com Seguranca: %u\n", global_pool.total_connections);

    return 0;
}
