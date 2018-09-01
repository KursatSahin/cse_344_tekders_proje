//
// Created by ksahin on 27.05.2018.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#ifndef C_SAMPLES_KSNVECTOR_H
#define C_SAMPLES_KSNVECTOR_H

typedef struct {
    int size;
    int capacity;
    void **data;
}vector;

void vector_init(vector *vec);

int vector_get_size(vector * vec);

bool vector_resize(vector *vec, int newcapacity);

void * vector_get(vector* vec, int index);

void * vector_get_first(vector* vec);

void * vector_get_last(vector* vec);

void * vector_remove(vector* vec, int index);

bool vector_add(vector *vec, void * item);

void vector_free(vector *vec);

#endif //C_SAMPLES_KSNVECTOR_H
