
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>

#define TAM 5

int buffer[TAM];
int entrada = 0;
int salida = 0;
int dato = 1;
sem_t *espacios;
sem_t *elementos;
pthread_mutex_t mutex;
void *productor(void *arg)
{
    int num = *(int *)arg;

    for (int i = 0; i < 5; i++)
    {
        sem_wait(espacios);
        pthread_mutex_lock(&mutex);
        int producto = dato++;
        buffer[entrada] = producto;
        printf("Productor %d produjo: %d\n", num, producto);
        entrada = (entrada + 1) % TAM;
        pthread_mutex_unlock(&mutex);
        sem_post(elementos);
        sleep(1);
    }
    pthread_exit(NULL);
}

void *consumidor(void *arg)
{
    int num = *(int *)arg;
    for (int i = 0; i < 5; i++)
    {
        sem_wait(elementos);
        pthread_mutex_lock(&mutex);
        int producto = buffer[salida];
        printf("Consumidor %d consumio: %d\n", num, producto);
        salida = (salida + 1) % TAM;
        pthread_mutex_unlock(&mutex);
        sem_post(espacios);
        sleep(1);
    }
    pthread_exit(NULL);
}

int main()
{
    pthread_t productores[2];
    pthread_t consumidores[2];
    int numP[2] = {1, 2};
    int numC[2] = {1, 2};
    espacios = sem_open("/espacios", O_CREAT, 0644, TAM);
    elementos = sem_open("/elementos", O_CREAT, 0644, 0);
    if (espacios == SEM_FAILED || elementos == SEM_FAILED)
    {
        perror("Error al crear semaforos");
        return 1;
    }
    pthread_mutex_init(&mutex, NULL);
    pthread_create(&productores[0], NULL, productor, &numP[0]);
    pthread_create(&productores[1], NULL, productor, &numP[1]);
    pthread_create(&consumidores[0], NULL, consumidor, &numC[0]);
    pthread_create(&consumidores[1], NULL, consumidor, &numC[1]);
    for (int i = 0; i < 2; i++)
    {
        pthread_join(productores[i], NULL);
        pthread_join(consumidores[i], NULL);
    }
    sem_close(espacios);
    sem_close(elementos);
    sem_unlink("/espacios");
    sem_unlink("/elementos");
    pthread_mutex_destroy(&mutex);
    return 0;
}
