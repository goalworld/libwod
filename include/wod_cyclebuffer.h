/*
 * cycle_buffer.h
 *
 *  Created on: 2012-11-11
 *      Author: goalworld
 */

#ifndef _WOD_CYCLE_BUFFER_H_
#define _WOD_CYCLE_BUFFER_H_
#include <stdbool.h>
struct wodCycleBuf
{
	char *buf;
	int cap;
	int head;
	int tail;
};
struct wodCyclePair
{
	struct {
		void *buf;
		int sz;
	}first,second;
};
int wodCycleBufInit( struct wodCycleBuf* cycle,int defsize);
void wodCycleBufDestroy( struct wodCycleBuf* cycle );
//growsz =0 剩下的空间（除了tail标记的）都将被设置为使用
int wodCycleBufGrow(struct wodCycleBuf* cycle,int growsz,struct wodCyclePair *pair);
void wodCycleBufBack(struct wodCycleBuf* cycle,int backsz);
int wodCycleBufPush(struct wodCycleBuf* cycle,void *buf,int sz);
void wodCycleBufPop(struct wodCycleBuf* cycle,int sz);
int wodCycleBufUsed(struct wodCycleBuf* cycle,struct wodCyclePair * pair);


bool wodCycleBufEmpty( struct wodCycleBuf* cycle );
#endif /* CYCLE_BUFFER_H_ */
