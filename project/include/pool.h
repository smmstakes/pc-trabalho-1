#pragma once

#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

/* Maximum number of simultaneous connections in the pool */
#define MAX_POOL_SIMULTANEOUS_CONNECTIONS 13

/* Maximum number of requests from same type */
#define MAX_POOL_REQUESTS 7

/* Maximum number of connectinos in the pool */
#define MAX_CONNECTIONS 127

/**
 * @brief Type definition for the connection pool
 */
typedef struct
{
    pthread_mutex_t lock; /* Mutex to protect the pool */

    pthread_cond_t cond_readers; /* Condition variable for readers */
    pthread_cond_t cond_writers; /* Condition variable for writers */

    sem_t sem_connections; /* Semaphore to limit the number of connections */

    uint8_t active_readers; /* Number of active readers */
    uint8_t active_writers; /* Number of active writers */

    uint8_t waiting_readers; /* Number of waiting readers */
    uint8_t waiting_writers; /* Number of waiting writers */

    uint8_t consecutive_readers; /* Number of consecutive readers */
    uint8_t consecutive_writers; /* Number of consecutive writers */

    uint32_t total_connections; /* Total number of connections in the pool */
    bool is_shutting_dowm;      /** Flag to symbolize that the pool is shutting down  */

} connection_pool_t;

/**
 * @brief Initialize the mutexes, condition variables and semaphore from the
 * connection pool
 *
 * @param pool Pointer to the shared connection pool
 */
void pool_init(connection_pool_t *pool);

/**
 * @brief Destroy all the resources created by pool
 *
 * @param poolPointer to the shared connection pool
 */
void pool_destroy(connection_pool_t *pool);

/**
 * @brief Adds a read process to the connection pool flow
 *
 * @param pool Pointer to the shared connection pool
 * @param thread_id The unique identifier of the requesting thread
 */
bool enter_read_pool(connection_pool_t *pool, const uint8_t thread_id);

/**
 * @brief Exit a read process of the connection pool flow
 *
 * @param pool Pointer to the shared connection pool
 * @param thread_id The unique identifier of the requesting thread
 */
void exit_read_pool(connection_pool_t *pool, const uint8_t thread_id);

/**
 * @brief Adds a write process to the connection pool flow
 *
 * @param pool Pointer to the shared connection pool
 * @param thread_id The unique identifier of the requesting thread
 */
bool enter_write_pool(connection_pool_t *pool, const uint8_t thread_id);

/**
 * @brief Exit a write process of the connection pool flow
 *
 * @param pool Pointer to the shared connection pool
 * @param thread_id The unique identifier of the requesting thread
 */
void exit_write_pool(connection_pool_t *pool, const uint8_t thread_id);
