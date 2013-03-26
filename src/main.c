#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <alsa/asoundlib.h>
#include <pthread.h>

struct vahn_packet {
  uint8_t type;
  uint8_t _padding;
  uint8_t data[114];
} __attribute__((packed));

uint16_t peer_addr;

void* cap_thread(void* param){
  struct vahn_packet packet_out = {0};

  snd_pcm_t* pcm_handle;
  snd_pcm_stream_t pcm_stream = SND_PCM_STREAM_CAPTURE;
  snd_pcm_hw_params_t* pcm_hwparams;
  char* pcm_name = strdup("plughw:0,0");

  packet_out.type = 19;

  snd_pcm_hw_params_alloca(&pcm_hwparams);
  snd_pcm_open(&pcm_handle, pcm_name, pcm_stream, 0);

  snd_pcm_hw_params_any(pcm_handle, pcm_hwparams);
  snd_pcm_hw_params_set_access(pcm_handle, pcm_hwparams, SND_PCM_ACCESS_RW_INTERLEAVED); //Interleaved
  snd_pcm_hw_params_set_format(pcm_handle, pcm_hwparams, SND_PCM_FORMAT_S16_LE);         //16bit samples
  snd_pcm_hw_params_set_rate(pcm_handle, pcm_hwparams, 8000, 0);                         //8000hz
  snd_pcm_hw_params_set_channels(pcm_handle, pcm_hwparams, 1);                           //Mono
  snd_pcm_hw_params_set_periods(pcm_handle, pcm_hwparams, 2, 0);                         //Double buffer
  snd_pcm_hw_params_set_buffer_size(pcm_handle, pcm_hwparams, 228);

  snd_pcm_hw_params(pcm_handle, pcm_hwparams);
  
  while(1){
    if(snd_pcm_readi(pcm_handle,&packet_out.data,57) < 0){
      snd_pcm_prepare(pcm_handle);
      continue;
    }

    sahn_send(peer_addr,packet_out,116);
  }
}

void* play_thread(void* param){
  struct vahn_packet packet_in = {0};

  snd_pcm_t* pcm_handle;
  snd_pcm_stream_t pcm_stream = SND_PCM_STREAM_PLAYBACK;
  snd_pcm_hw_params_t* pcm_hwparams;
  char* pcm_name = strdup("plughw:0,0");

  snd_pcm_hw_params_alloca(&pcm_hwparams);
  snd_pcm_open(&pcm_handle, pcm_name, pcm_stream, 0);

  snd_pcm_hw_params_any(pcm_handle, pcm_hwparams);
  snd_pcm_hw_params_set_access(pcm_handle, pcm_hwparams, SND_PCM_ACCESS_RW_INTERLEAVED); //Interleaved
  snd_pcm_hw_params_set_format(pcm_handle, pcm_hwparams, SND_PCM_FORMAT_S16_LE);         //16bit samples
  snd_pcm_hw_params_set_rate(pcm_handle, pcm_hwparams, 8000, 0);                         //8000hz
  snd_pcm_hw_params_set_channels(pcm_handle, pcm_hwparams, 1);                           //Mono
  snd_pcm_hw_params_set_periods(pcm_handle, pcm_hwparams, 2, 0);                         //Double buffer
  snd_pcm_hw_params_set_buffer_size(pcm_handle, pcm_hwparams, 228);

  snd_pcm_hw_params(pcm_handle, pcm_hwparams);

  while(1){
    sahn_recv(NULL,packet_in,116);

    switch(packet_in.type){
    case 3:
      //TODO
      break;
    case 19:
      while(snd_pcm_writei(pcm_handle,&packet_in.data,57) < 0){
	snd_pcm_prepare(pcm_handle);
      }
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

  sahn_init(argv[1],atoi(argv[2]));

  if(argc > 3){
    peer_addr = atoi(argv[3]);
    packet_out.type = 1;

    sahn_send(peer_addr,&packet_out,1);

    do {
      sahn_recv(&addr_in,&packet_in,116);
    } while(addr_in != peer_addr && packet_in.type != 2);
  } else {
    do {
      sahn_recv(&addr_in,&packet_in,116);
    } while(packet_in.type != 1);

    peer_addr = addr_in;
    packet_out.type = 2;

    sahn_send(peer_addr,&packet_out,1);
  }

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
