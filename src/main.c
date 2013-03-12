#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct vahn_packet {
  uint8_t type;
  uint8_t data[115];
};

int main(int argc, char** argv){
  if(argc < 3 || argc > 4){
    printf("Usage: vahn <node-file> <address> [peer-address]\n");
    exit(1);
  }

  uint16_t peer_addr, addr_in;
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
