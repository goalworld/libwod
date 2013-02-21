/*
 * wod_array.h
 *
 *  Created on: 2012-11-14
 *      Author: goalworld
 */

#ifndef _WOD_ARRAY_H_
#define _WOD_ARRAY_H_


struct wod_array
{
	void *arr_data;
	int elem_sz;
	int arr_sz;
	int cut_sz;
};
void wod_array_init(struct wod_array *arr,int elemsz);
void wod_array_destroy(struct wod_array *arr);
void wod_array_push(struct wod_array *arr,void *elem);
void wod_array_at(struct wod_array *arr,int index,void *elem);
void wod_array_set(struct wod_array *arr,int index,void *elem);
void wod_array_unshift(struct wod_array *arr,void *elem);
void wod_array_earse(struct wod_array *arr,int index,int numelem,void *elem);
void wod_array_insert(struct wod_array *arr,int index,void *elem,int numelem);
unsigned wod_array_size(struct wod_array *arr);
#endif /* RAIN_ARRAY_H_ */
