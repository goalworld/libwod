/*
 * rain_array.h
 *
 *  Created on: 2012-11-14
 *      Author: goalworld
 */

#ifndef RAIN_ARRAY_H_
#define RAIN_ARRAY_H_

typedef struct wcArray wcArray;

struct wcArray
{
	void *arr_data;
	int elem_sz;
	int arr_sz;
	int cut_sz;
};

void wcArrayNew(wcArray *arr,int elemsz);
void wcArrayDel(wcArray *arr);
void wcArrayPush(wcArray *arr,void *elem);
void wcArrayAt(wcArray *arr,int index,void *elem);
void wcArraySet(wcArray *arr,int index,void *elem);
void wcArrayUnShift(wcArray *arr,void *elem);
void wcArrayErase(wcArray *arr,int index,int numelem,void *elem);
void wcArrayInsert(wcArray *arr,int index,void *elem,int numelem);
int wcArraySize(wcArray *arr);
#endif /* RAIN_ARRAY_H_ */
