#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include <pulse/def.h>
#include <pulse/simple.h>

struct vahn_packet {
  uint8_t type;
  uint8_t _padding;
  uint8_t data[114];
} __attribute__((packed));

uint16_t peer_addr;

int init_audio(pa_simple** handle, int pa_stream){
  pa_sample_spec ss;
  
  ss.format = PA_SAMPLE_S16NE;
  ss.channels = 1;
  ss.rate = 8000;
  
  *handle = pa_simple_new(NULL,"Voice over AdHoc Network",pa_stream,NULL,"Music",&ss,NULL,NULL,NULL);
}

void* cap_thread(void* param){
  struct vahn_packet packet_out = {.type = 19};

  pa_simple* pa_handle;

  init_audio(&pa_handle,PA_STREAM_RECORD);

  while(1){
    pa_simple_read(pa_handle,&packet_out.data,114,NULL);
    sahn_send(peer_addr,&packet_out,116);
  }
}

void* play_thread(void* param){
  struct vahn_packet packet_in = {0};

  pa_simple* pa_handle;

  init_audio(&pa_handle,PA_STREAM_PLAYBACK);

  while(1){
    sahn_recv(NULL,&packet_in,116);

    switch(packet_in.type){
    case 3:
      //TODO
      break;
    case 19:
      pa_simple_write(pa_handle,packet_in.data,114,NULL);
      break;
    }
  }
}

int main(int argc, char** argv){
  if(argc < 3 || argc > 4){
    printf("Usage: vahn <node-file> <address> [peer-address]\n");
    exit(1);
  }
  
  char buf[64];
  uint16_t addr_in;
  struct vahn_packet packet_out = {0}, packet_in = {0};
  pthread_t play, cap;

  sahn_init(argv[1],atoi(argv[2]),NULL);

  if(argc > 3){
    peer_addr = atoi(argv[3]);
  } else {
    sahn_recv(&addr_in,&packet_in,116);
    peer_addr = addr_in;
  }
  printf("Connected?\n");

  pthread_create(&play,NULL,play_thread,NULL);
  pthread_create(&cap,NULL,cap_thread,NULL);

  while(1){
    scanf("%s\n",buf);
    if(strcmp("stop",buf) == 0){
      pthread_cancel(play);
      pthread_cancel(cap);

      pthread_join(play,NULL);
      pthread_join(cap,NULL);
      break;
    }
  }

  sahn_cleanup();
  return 0;
}
