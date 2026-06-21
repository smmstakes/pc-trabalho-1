#include "worker.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void simulate_read_operation(const uint8_t thread_id)
{
    printf("[Thread %u] (BI) -> Executando query...\n", thread_id);
    sleep(3);
}

void simulate_write_operation(const uint8_t thread_id)
{
    printf("[Thread %u] (ETL) -> Executando bulk insert no banco...\n", thread_id);
    sleep(5);
}

void *bi_reader_worker(void *arg)
{
    worker_data_t *data = (worker_data_t *)arg;

    sleep(rand() % 21);

    enter_read_pool(data->pool, data->thread_id);
    simulate_read_operation(data->thread_id);
    exit_read_pool(data->pool, data->thread_id);

    free(data);

    return NULL;
}

void *etl_writer_worker(void *arg)
{
    worker_data_t *data = (worker_data_t *)arg;

    sleep(rand() % 21);

    enter_write_pool(data->pool, data->thread_id);
    simulate_write_operation(data->thread_id);
    exit_write_pool(data->pool, data->thread_id);

    free(data);

    return NULL;
}
