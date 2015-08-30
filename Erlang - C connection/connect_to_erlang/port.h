/*
 * port.h
 *
 *  Created on: Aug 23, 2015
 *      Author: barlesh
 */

#ifndef PORT_H_
#define PORT_H_

//#include "erl_comm.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef unsigned char byte;

int foo(int arg);
int bar(int arg);
int read_cmd(byte *buf);
int write_cmd(byte *buf, int len);
int read_exact(byte *buf, int len);
int write_exact(byte *buf, int len);

#endif /* PORT_H_ */
