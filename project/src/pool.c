#include "pool.h"

#include <stdio.h>
#include <stdlib.h>

void pool_init(connection_pool_t *pool)
{
    if (pthread_mutex_init(&pool->lock, NULL) != 0)
    {
        perror("Erro ao inicializar o mutex!\n");
        exit(EXIT_FAILURE);
    }

    pthread_cond_init(&pool->cond_readers, NULL);
    pthread_cond_init(&pool->cond_writers, NULL);

    if (sem_init(&pool->sem_connections, 0, MAX_POOL_SIMULTANEOUS_CONNECTIONS) != 0)
    {
        perror("Erro ao inicializar o semaforo!\n");
        exit(EXIT_FAILURE);
    }

    pool->active_readers = 0;
    pool->active_writers = 0;

    pool->waiting_readers = 0;
    pool->waiting_writers = 0;

    pool->consecutive_readers = 0;
    pool->consecutive_writers = 0;

    pool->total_connections = 0;
    pool->is_shutting_dowm = false;
}

void pool_destroy(connection_pool_t *pool)
{
    pthread_mutex_destroy(&pool->lock);

    pthread_cond_destroy(&pool->cond_readers);
    pthread_cond_destroy(&pool->cond_writers);

    sem_destroy(&pool->sem_connections);
}

bool enter_read_pool(connection_pool_t *pool, const uint8_t thread_id)
{
    pthread_mutex_lock(&pool->lock);

    if (pool->is_shutting_dowm)
    {
        pthread_mutex_unlock(&pool->lock);
        return false;
    }

    bool quota_exhausted = (pool->consecutive_readers >= MAX_POOL_REQUESTS && pool->waiting_writers > 0);

    while (pool->active_writers > 0 || quota_exhausted)
    {
        pool->waiting_readers++;

        printf("[Thread %u] (Leitor) Aguardando...\n", thread_id);
        printf("Escritores Ativos: %u.\n", pool->active_writers);

        pthread_cond_wait(&pool->cond_readers, &pool->lock);
        pool->waiting_readers--;

        if (pool->is_shutting_dowm)
        {
            pthread_mutex_unlock(&pool->lock);
            return false;
        }

        quota_exhausted = (pool->consecutive_readers >= MAX_POOL_REQUESTS && pool->waiting_writers > 0);
    }

    pool->active_readers++;
    pool->consecutive_readers++;
    pool->total_connections++;

    pool->consecutive_writers = 0;

    if (pool->total_connections >= MAX_CONNECTIONS)
    {
        pool->is_shutting_dowm = true;

        pthread_cond_broadcast(&pool->cond_readers);
        pthread_cond_broadcast(&pool->cond_writers);
    }

    pthread_mutex_unlock(&pool->lock);

    sem_wait(&pool->sem_connections);

    printf("[Thread %u] (Leitor) Conexão adquirida...\n", thread_id);

    return true;
}

bool enter_write_pool(connection_pool_t *pool, const uint8_t thread_id)
{
    pthread_mutex_lock(&pool->lock);

    if (pool->is_shutting_dowm)
    {
        pthread_mutex_unlock(&pool->lock);
        return false;
    }

    bool quota_exausted = (pool->consecutive_writers >= MAX_POOL_REQUESTS && pool->waiting_readers > 0);

    while (pool->active_readers > 0 || quota_exausted)
    {
        pool->waiting_writers++;

        printf("[Thread %u] (Escritor) Aguardando...\n", thread_id);
        printf("Leitores Ativos: %u.\n", pool->active_readers);

        pthread_cond_wait(&pool->cond_writers, &pool->lock);
        pool->waiting_writers--;

        if (pool->is_shutting_dowm)
        {
            pthread_mutex_unlock(&pool->lock);
            return false;
        }

        quota_exausted = (pool->consecutive_writers >= MAX_POOL_REQUESTS && pool->waiting_readers > 0);
    }

    pool->active_writers++;
    pool->consecutive_writers++;
    pool->total_connections++;

    pool->consecutive_readers = 0;

    if (pool->total_connections >= MAX_CONNECTIONS)
    {
        pool->is_shutting_dowm = true;

        pthread_cond_broadcast(&pool->cond_readers);
        pthread_cond_broadcast(&pool->cond_writers);
    }

    pthread_mutex_unlock(&pool->lock);

    sem_wait(&pool->sem_connections);

    printf("[Thread %u] (Escritor) Conexão adquirida...\n", thread_id);

    return true;
}

void exit_read_pool(connection_pool_t *pool, const uint8_t thread_id)
{
    sem_post(&pool->sem_connections);

    pthread_mutex_lock(&pool->lock);
    pool->active_readers--;

    printf("[Thread %u] (Leitor) Liberou conexão do pool.\n", thread_id);
    printf("Leitores Ativos Restantes: %u\n", pool->active_readers);

    if (pool->active_readers == 0)
    {
        pthread_cond_broadcast(&pool->cond_writers);
    }

    pthread_mutex_unlock(&pool->lock);
}

void exit_write_pool(connection_pool_t *pool, const uint8_t thread_id)
{
    sem_post(&pool->sem_connections);

    pthread_mutex_lock(&pool->lock);
    pool->active_writers--;

    printf("[Thread %u] (Escritor) Liberou conexão do pool.\n", thread_id);
    printf("Escritores Ativos Restantes: %u\n", pool->active_writers);

    if (pool->active_writers == 0)
    {
        pthread_cond_broadcast(&pool->cond_readers);
    }

    pthread_mutex_unlock(&pool->lock);
}