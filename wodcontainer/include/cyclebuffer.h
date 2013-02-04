/*
 * cycle_buffer.h
 *
 *  Created on: 2012-11-11
 *      Author: goalworld
 */

#ifndef CYCLE_BUFFER_H_
#define CYCLE_BUFFER_H_
#include <stdbool.h>
typedef struct cycle_buffer_s
{
	char *buf;
	int cap;
	int head;
	int tail;
}cycle_buffer_t;
typedef struct cycle_pair_s
{
	struct {
		void *buf;
		int sz;
	}first,second;
}cycle_pair_t;
int wcCycleBufNew( cycle_buffer_t* cycle,int defsize);
void wcCycleBufDel( cycle_buffer_t* cycle );
//growsz =0 剩下的空间（除了tail标记的）都将被设置为使用
int wcCycleBufGrow(cycle_buffer_t* cycle,int growsz,cycle_pair_t *pair);
void wcCycleBufBack(cycle_buffer_t* cycle,int backsz);
int wcCycleBufPush(cycle_buffer_t* cycle,void *buf,int sz);
void wcCycleBufPop(cycle_buffer_t* cycle,int sz);
int wcCycleBufUsed(cycle_buffer_t* cycle,cycle_pair_t * pair);


bool wcCycleBufEmpty( cycle_buffer_t* cycle );
#endif /* CYCLE_BUFFER_H_ */
