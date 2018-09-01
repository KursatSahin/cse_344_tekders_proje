//
// Created by ksahin on 27.05.2018.
//

#include "ksnVector.h"

void vector_init(vector *vec){
    vec->capacity = 2;
    vec->size = 0;
    vec->data= calloc(vec->capacity,sizeof(void *));
    if (vec->data == NULL) {
        fprintf(stderr,"calloc() retuns NULL error in vector_init()");
    }
}

int vector_get_size(vector * vec){
    return vec->size;
}

bool vector_resize(vector *vec, int newcapacity){
    vec->capacity = newcapacity;
    vec->data = realloc(vec->data, newcapacity * sizeof(void*));
    if (vec->data == NULL){
        fprintf(stderr,"recalloc() error in vector_resize().");
        return false;
    }
    return true;
}

void * vector_get(vector* vec, int index){
    if (index >= 0 && index < vec->size ){
        return vec->data[index];
    } else {
        fprintf(stderr,"try to access to invalid index error in vector_get().");
        return NULL;
    }
}

void * vector_get_first(vector* vec){
    return vector_get (vec, 0);
}

void * vector_get_last(vector* vec) {
    return vector_get (vec, vector_get_size(vec));
}

void * vector_remove(vector* vec, int index){
    void * item;

    if (index >= 0 && index < vec->size ) {
        item = vector_get(vec,index);
        vec->data[index] = NULL;
        vec->size--;

        for (int i = index; i < vec->size; ++i) {
            if (index == vec->size-1){
                vec->data[i] = NULL;
            } else {
                vec->data[i] = vec->data[i+1];
            }
        }
        return item;
    }
    fprintf(stderr,"try to access to invalid index error in vector_remove().");
    return NULL;
}

bool vector_add(vector *vec, void * item){

    if (vec->capacity == vec->size) {
        if (!vector_resize(vec, vec->capacity * 2)) {
            fprintf(stderr, "vector_resize() error in vector_add().");
            return false;
        }
    } else if (vec->capacity < vec->size){
        fprintf(stderr,"vector->capacity is smaller than size error in vector_add().");
        return false;
    }

    vec->data[vec->size++] = item;
    return true;
}

void vector_free(vector *vec){
    free(vec->data);
}