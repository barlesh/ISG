/*
 * erl_comm.h
 *
 *  Created on: Aug 23, 2015
 *      Author: barlesh
 */

#ifndef ERL_COMM_H_
#define ERL_COMM_H_

#include <unistd.h>

typedef unsigned char byte;


int read_cmd(byte *buf);
int write_cmd(byte *buf, int len);
int read_exact(byte *buf, int len);
int write_exact(byte *buf, int len);
//int what(int why);


#endif /* ERL_COMM_H_ */
