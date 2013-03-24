#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <alsa/asoundlib.h>

struct vahn_packet {
  uint8_t type;
  uint8_t data[115];
};

uint16_t peer_addr;

void cap_thread(void* param){
  
}

void play_thread(void* param){
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
  snd_pcm_hw_params_set_periods(pcm_handle, pcm_hwparams, 2, 0);
  
}

int main(int argc, char** argv){
  if(argc < 3 || argc > 4){
    printf("Usage: vahn <node-file> <address> [peer-address]\n");
    exit(1);
  }
  
  uint16_t addr_in;
  struct vahn_packet packet_out = {0}, packet_in = {0};

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

  sahn_cleanup();
  return 0;
}
