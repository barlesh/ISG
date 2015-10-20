#include "port7.h"

#define DEBUG		0



void main(){

		unsigned short debug_buff[PACKET_SIZE];
   
    configure_terminal();
    
    if(DEBUG)
    	create_debug_packet(debug_buff, 3);
    
    while(1){
    	if(!DEBUG){
    		wait_for_input();
    		handle_input();
    		//handle_input_from_modem();
    		}
    		
    	
    	/*or.....*/
    	/*debug mode only. at full version packets are sent after getting input from Erlang*/
    	if(DEBUG){
    			sleep(1);
    			send_to_Modem(debug_buff, PACKET_SIZE);	
    	}	
    	/*end of debug*/

    
    }//while(1)
 }//main 	
 
 
/*this function suspend the process execution until input is available from one of readfs inserted stream/
**at this case:
**0 - stdin - Erlang port's way to communicate with this c based process
**terminal_fd - Edison's Serial 3(?) input buffer (Packet from Modem)*/
void wait_for_input(){
   		if(terminal_fd<0) { printf("error. terminal_fd was not initiated\n"); return; }
   		configure_input_select();
   		printf("waiting for input (terminal_fd is:%d\n", terminal_fd);
   		/*this function suspend process until input is availabl*/
   		select(terminal_fd + 1, &readfs, NULL, NULL, NULL);
   		printf("passed select\n");
   		
}
  
/*this function is executed only if wait_for_input() returnd, which means input is available.
**it check which input is ready, and act as follows:
** input from stdin - Erlang app send a Packet -> send it through Terminal
**input from terminal_fd - Modem sent a Packet -> send it to Erlang App through stdin*/ 
void handle_input(){
	   	if (FD_ISSET(0, &readfs))         /* input from source 1 available - Erlang App is sending packet  */
           handle_input_from_erlang();
      if (FD_ISSET(terminal_fd, &readfs))         /* input from source 2 available - Modem is sending packet */
          	handle_input_from_modem();

} 	


/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%			Erlang Related Functions		%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

void handle_input_from_erlang(){
	return;
}

void send_to_erlang(unsigned short* buffer, short len){
	return;
}
 
 
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%			Modem Related Functions		%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/  

void handle_input_from_modem(){
	int i=0, counter=0, short_byte;
	unsigned short buffer[PACKET_SIZE], byte;
	short len = 1;
	printf("trying to read from modem\n");
	usleep(10);
	
	do{
		len = read(terminal_fd,&byte,1);
		if(len<1) {printf("didnt read nothing\n");}
		else{
			short_byte = byte & 0xFF;
			printf("got byte number %d. byte:%d\n", i, short_byte);
			i++;
			counter++;
		}//else
	usleep(30);
	}while(len==1 && counter<PACKET_SIZE);
	/*
	for(i=0; i<PACKET_SIZE; i++){
		len = read(terminal_fd,&byte,1);
		if(len==1){ 
			short_byte = byte & 0xFF;
			printf("got byte number %d. byte:%d\n", i, short_byte);
			buffer[i] = byte;
			counter++;
			}//if
		else{
			printf("error reading from modem\n");
			exit(-1);
		}//else
	}//for
	//or.....
	/*len = read(terminal_fd,buffer,PACKET_SIZE);*/
	
	//printf("got data from modem. len:%d. packet is:", counter);
	//show_buffer(buffer, counter);
	send_to_erlang(buffer, counter);		//TODO\
	
	

}

void send_to_Modem(unsigned short * buffer, short len){
	int i=0;
	for(i=0; i<len;i++)
		write(terminal_fd, &buffer[i], 1);
	//write(terminal_fd, buffer, len);
	printf("sent Modem a Packet. len:%d. Packet is:\n", len);
	show_buffer(buffer,len);

}


void configure_terminal(){
	int rc;
	
	/*Terminal configuration*/
	terminal_fd = open(MODEMDEVICE, O_RDWR | O_NOCTTY ); 
	
 /* Check if the file descriptor is pointing to a TTY device or not*/
 	if(!isatty(terminal_fd)) { printf("error1\n"); }
	if (terminal_fd <0) {perror(MODEMDEVICE); printf("error2\n"); }
	
	
	if((rc = tcgetattr(terminal_fd, &newtio)) < 0){
        fprintf(stderr, "failed to get attr: %d, %s\n", terminal_fd, strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Set the baud rates to 
    cfsetispeed(&newtio, BAUDRATE);

    // Set the baud rates to 
    cfsetospeed(&newtio, BAUDRATE);

    cfmakeraw(&newtio);
    newtio.c_cflag |= (CLOCAL | CREAD);   // Enable the receiver and set local mode
    newtio.c_cflag &= ~CSTOPB;            // 1 stop bit
    newtio.c_cflag &= ~CRTSCTS;           // Disable hardware flow control
    newtio.c_cc[VMIN]  = 66;
    newtio.c_cc[VTIME] = 1;

    // Set the new attributes
    if((rc = tcsetattr(terminal_fd, TCSANOW, &newtio)) < 0){
        fprintf(stderr, "failed to set attr: %d, %s\n", terminal_fd, strerror(errno));
        exit(EXIT_FAILURE);
    }


}



/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%			Other		%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
void configure_input_select(){
	   /*select() configuration*/
   	FD_ZERO(&readfs);
    FD_SET(0, &readfs);  /* set testing for source 1 - sdtin */
    FD_SET(terminal_fd, &readfs);  /* set testing for source 2 - terminal_fd */
}

void show_buffer(unsigned short *buffer, short len){
	int i=0, n=0;
	//printf("len is:%d\n", len);
	for(i;i<len; i++){
		n = buffer[i];
		n = n % 0xFF;
		printf("(%d)%d_",i, n);
		}
	printf("\n");

}




void create_debug_packet(unsigned short * buffer, int type){
	int i = 2;
	unsigned short c = 1;
	buffer[0] = type;
	buffer[1] = 0 ;
	for(i=2; i<PACKET_SIZE; i++)
		buffer[i] = ++c;
}





















/*




void configure_terminal1(){
		int retval;
		terminal_fd = open(MODEMDEVICE, O_RDWR | O_NDELAY);
    retval = tcgetattr(terminal_fd, &newtio);
    if(retval != 0)
    {
        perror(MODEMDEVICE);
        exit(-1);
    }

    cfsetospeed(&newtio, BAUDRATE);
    cfsetispeed(&newtio, BAUDRATE);

    newtio.c_cflag = (newtio.c_cflag & ~CSIZE) | CS8;
    newtio.c_cflag |= CLOCAL | CREAD;
    newtio.c_cflag &= ~(PARENB | PARODD | ICANON);
    newtio.c_cflag &= ~CRTSCTS;
    newtio.c_cflag &= ~CSTOPB;

    newtio.c_iflag = IGNBRK;
    newtio.c_iflag &= ~(IXON | IXOFF | IXANY);

    newtio.c_lflag = 0;

    newtio.c_oflag = 0;

    newtio.c_cc[VTIME] = 1;
    newtio.c_cc[VMIN] = 66;


    retval = tcsetattr(terminal_fd, TCSANOW, &newtio);
    if(retval != 0)
    {
        perror(MODEMDEVICE);
        exit(-1);
    }
    printf("Init 4 complete.\n");


}*/




void configure_terminal4(){ 
	/*Terminal configuration*/
	terminal_fd = open(MODEMDEVICE, O_RDWR | O_NOCTTY ); 
	
 /* Check if the file descriptor is pointing to a TTY device or not*/
 	if(!isatty(terminal_fd)) { printf("error1\n"); }
	if (terminal_fd <0) {perror(MODEMDEVICE); printf("error2\n"); }
	
	/* save current port settings */
	tcgetattr(terminal_fd,&oldtio); 
	
	/*get another copy of current settings, to be changed*/
	if(tcgetattr(terminal_fd, &newtio) < 0) { printf("error3\n"); }
	
	/*general configuration*/
 	//newtio.c_cflag &= ~( /*CSIZE |*/ CRTSCTS | PARENB | CSTOPB | HUPCL);
 	//newtio.c_lflag = 0;
  newtio.c_lflag |= ( CS8 | CLOCAL | CREAD);
  
   //
 // Communication speed (simple version, using the predefined
 // constants)
 //
 if(cfsetispeed(&newtio, BAUDRATE) < 0 || cfsetospeed(&newtio, BAUDRATE) < 0) {
     printf("error4\n");
 }
  
  /*input configuration*/
  /*newtio.c_iflag |= (IGNBRK | IGNPAR | IGNCR);
  newtio.c_iflag &= ~(  BRKINT | ICRNL |
                     INLCR | PARMRK | INPCK | ISTRIP | IXON | IXOFF);*/
  //or..... 
  //newtio.c_iflag = IGNPAR;
  
  /*output configuration*/
  //newtio.c_oflag |= ( );
  //newtio.c_oflag &= 0;
  
   /*input mode (non-canonical, no echo,...) */
   //newtio.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);
   //newtio.c_lflag=0;
   
   /*terminal special characters*/      
   //newtio.c_cc[VTIME]    = 0;   /* inter-character timer - 1 * 0.1 [sec] */
   //newtio.c_cc[VMIN]     = 66;   /* blocking read until 66 chars received */
        
   tcflush(terminal_fd, TCIFLUSH);
   tcsetattr(terminal_fd,TCSANOW,&newtio);
}

void configure_terminal2(){
        int c, res;
        //struct termios oldtio,newtio;
        char buf[255];
        
        terminal_fd = open(MODEMDEVICE, O_RDWR | O_NOCTTY ); 
        if (terminal_fd <0) {perror(MODEMDEVICE); exit(-1); }
        
        tcgetattr(terminal_fd,&oldtio); /* save current port settings */
        
        bzero(&newtio, sizeof(newtio));
        newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
        newtio.c_iflag = IGNPAR;
        newtio.c_oflag = 0;
        
        /* set input mode (non-canonical, no echo,...) */
        newtio.c_lflag = 0;
         
        newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
        newtio.c_cc[VMIN]     = 66;   /* blocking read until 5 chars received */
        
        tcflush(terminal_fd, TCIFLUSH);
        tcsetattr(terminal_fd,TCSANOW,&newtio);
}

