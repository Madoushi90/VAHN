#include <stdlib.h>
#include <sahn.h>
#include <pthread.h>

int main(int argc, char** argv){
  pthread_mutex_t lock;
  pthread_cond_t wait;
  pthread_mutex_init(&lock,NULL);
  pthread_cond_init(&wait,NULL);

  sahn_init(argv[1],atoi(argv[2]));

  pthread_cond_wait(&wait,&lock);

  sahn_cleanup();

  pthread_mutex_destroy(&lock);
  pthread_cond_destroy(&wait);

  return 0;
}
