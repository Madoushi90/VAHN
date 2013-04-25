#include <stdlib.h>
#include <signal.h>
#include <sahn.h>
#include <pthread.h>

pthread_mutex_t lock;
pthread_cond_t wait;

void shutdown(int sig){
  pthread_cond_broadcast(&wait);
}

int main(int argc, char** argv){
  signal(SIGINT,&shutdown);

  pthread_mutex_init(&lock,NULL);
  pthread_cond_init(&wait,NULL);

  sahn_init(argv[1],atoi(argv[2]),NULL);

  pthread_cond_wait(&wait,&lock);

  sahn_cleanup();

  pthread_mutex_destroy(&lock);
  pthread_cond_destroy(&wait);

  return 0;
}
