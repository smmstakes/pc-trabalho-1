#pragma once

#include <stdint.h>

#include "pool.h"

/**
 * @brief Structure to hold data passed to each worker thread
 */
typedef struct
{
    uint8_t thread_id;      /* Unique identifier for the thread */
    connection_pool_t *pool; /* Pointer to the shared connection pool */
} worker_data_t;

/**
 * @brief Thread routine that simulates a Business Intelligence (BI) query.
 * Acts as a Reader in the Connection Pool.
 * * @param arg Pointer to dynamically allocated worker_data_t
 */
void *bi_reader_worker(void *arg);

/**
 * @brief Simulates a BI query execution
 *
 * @param thread_id The unique identifier of the requesting thread
 */
void simulate_read_operation(const uint8_t thread_id);

/**
 * @brief Thread routine that simulates an ETL bulk insert/update operation.
 * Acts as a Writer in the Connection Pool.
 * * @param arg Pointer to dynamically allocated worker_data_t
 */
void *etl_writer_worker(void *arg);

/**
 * @brief Simulates an ETL bulk insert or update
 *
 * @param thread_id The unique identifier of the requesting thread
 */
void simulate_write_operation(const uint8_t thread_id);
