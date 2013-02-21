/*
 * rain_array.h
 *
 *  Created on: 2012-11-14
 *      Author: goalworld
 */

#ifndef RAIN_ARRAY_H_
#define RAIN_ARRAY_H_


struct wcArray
{
	void *arr_data;
	int elem_sz;
	int arr_sz;
	int cut_sz;
};
#define wcArrayGetBuffer(parr) ((parr)->arr_data)
void wcArrayInit(struct wcArray *arr,int elemsz);
void wcArrayDestroy(struct wcArray *arr);
void wcArrayPush(struct wcArray *arr,void *elem);
void wcArrayAt(struct wcArray *arr,int index,void *elem);
void wcArraySet(struct wcArray *arr,int index,void *elem);
void wcArrayUnShift(struct wcArray *arr,void *elem);
void wcArrayErase(struct wcArray *arr,int index,int numelem,void *elem);
void wcArrayInsert(struct wcArray *arr,int index,void *elem,int numelem);
unsigned wcArraySize(struct wcArray *arr);
#endif /* RAIN_ARRAY_H_ */
