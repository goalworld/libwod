/*
 * cycle_buffer.h
 *
 *  Created on: 2012-11-11
 *      Author: goalworld
 */

#ifndef CYCLE_BUFFER_H_
#define CYCLE_BUFFER_H_
#include <stdbool.h>
struct wcCycleBuf
{
	char *buf;
	int cap;
	int head;
	int tail;
};
struct wcCyclePair
{
	struct {
		void *buf;
		int sz;
	}first,second;
};
int wcCycleBufInit( struct wcCycleBuf* cycle,int defsize);
void wcCycleBufDestroy( struct wcCycleBuf* cycle );
//growsz =0 剩下的空间（除了tail标记的）都将被设置为使用
int wcCycleBufGrow(struct wcCycleBuf* cycle,int growsz,struct wcCyclePair *pair);
void wcCycleBufBack(struct wcCycleBuf* cycle,int backsz);
int wcCycleBufPush(struct wcCycleBuf* cycle,void *buf,int sz);
void wcCycleBufPop(struct wcCycleBuf* cycle,int sz);
int wcCycleBufUsed(struct wcCycleBuf* cycle,struct wcCyclePair * pair);


bool wcCycleBufEmpty( struct wcCycleBuf* cycle );
#endif /* CYCLE_BUFFER_H_ */
