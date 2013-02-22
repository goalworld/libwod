/*
 * wod_array.h
 *
 *  Created on: 2012-11-14
 *      Author: goalworld
 */

#ifndef _WOD_ARRAY_H_
#define _WOD_ARRAY_H_


struct wodArray
{
	void *arr_data;
	int elem_sz;
	int arr_sz;
	int cut_sz;
};
#define wodArrayGetBuffer(parr) ((parr)->arr_data)
void wodArrayInit(struct wodArray *arr,int elemsz);
void wodArrayDestroy(struct wodArray *arr);
void wodArrayPush(struct wodArray *arr,void *elem);
void wodArrayAt(struct wodArray *arr,int index,void *elem);
void wodArraySet(struct wodArray *arr,int index,void *elem);
void wodArrayUnShift(struct wodArray *arr,void *elem);
void wodArrayErase(struct wodArray *arr,int index,int numelem,void *elem);
void wodArrayInsert(struct wodArray *arr,int index,void *elem,int numelem);
unsigned wodArraySize(struct wodArray *arr);
#endif /* RAIN_ARRAY_H_ */
