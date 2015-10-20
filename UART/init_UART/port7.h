#include <stdio.h>
#include <string.h>   //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>   //close
#include <arpa/inet.h>    //close
#include <sys/types.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
#include <termios.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdint.h>     // C99 fixed data types



#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyUSB0"  
#define TRUE   1
#define FALSE  0
#define PACKET_SIZE	66

int terminal_fd = -1;
struct termios oldtio,newtio;
fd_set readfs;


void wait_for_input();
void handle_input();

void handle_input_from_erlang();

void handle_input_from_modem();
void send_to_Modem(unsigned short *buffer, short len);

void configure_terminal();
void configure_input_select();

void show_buffer(unsigned short *buffer, short len);
void create_debug_packet(unsigned short * buffer, int type);
