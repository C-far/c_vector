#if defined __C_VECTOR_INCLUDED__
    #endinput
#endif
#define __C_VECTOR_INCLUDED__
#define __C_VECTOR_VERSION__ 0x210

#include <YSI_Coding\y_malloc>
#tryinclude <YSI_Data\y_iterate>

#define VECTOR_NULL                     (Vector:NO_ALLOC)
#define INVALID_VECTOR_INDEX            (cellmin)

enum _:E_VECTOR {
    Alloc:E_VECTOR_DATA,
    E_VECTOR_SIZE
}

enum _:E_VECTOR_SORT {
    E_VECTOR_SORT_ASC,
    E_VECTOR_SORT_DESC
}

#if defined _INC_y_iterate

iterfunc stock vector(&iterstate, cur, Vector:vector, array[] = cellmin, size = sizeof(array)) {
    #pragma unused cur
    if(array[0] == cellmin) {
        return vector_get_value(vector, iterstate++);
    } else {
        return vector_get_array(vector, iterstate++, array, size);
    }
}
#define iterstart@vector iterstate(INVALID_VECTOR_INDEX, 0)

iterfunc stock vector_reverse(&iterstate, cur, Vector:vector, array[] = cellmin, size = sizeof(array)) {
    #pragma unused cur
    if(iterstate == cellmax) iterstate = vector_get_size(vector) - 1;
    if(array[0] == cellmin) {
        return vector_get_value(vector, iterstate--);
    } else {
        return vector_get_array(vector, iterstate--, array, size);
    }
}
#define iterstart@vector_reverse iterstate(INVALID_VECTOR_INDEX, cellmax)

iterfunc stock array(&iterstate, cur, const array[], size = sizeof(array), size2 = sizeof(array)) {
    #pragma unused cur
    if(iterstate < size && iterstate < size2) {
        return array[iterstate++];
    }
    return INVALID_VECTOR_INDEX;
}
#define iterstart@array iterstate(INVALID_VECTOR_INDEX, 0)

iterfunc stock array_reverse(&iterstate, cur, const array[], size = sizeof(array), size2 = sizeof(array)) {
    #pragma unused cur
    if(iterstate == cellmax) iterstate = (size > size2 ? size2 : size) - 1;
    if(iterstate != -1) {
        return array[iterstate--];
    }
    return INVALID_VECTOR_INDEX;
}
#define iterstart@array_reverse iterstate(INVALID_VECTOR_INDEX, cellmax)

#endif

static stock vector_push_back_ptr(&Vector:vector, &Alloc:ptr, size = 0) {
    new 
        v[E_VECTOR];

    if(vector == VECTOR_NULL) {
        v[E_VECTOR_DATA] = malloc(E_VECTOR, true);
        v[E_VECTOR_SIZE] = E_VECTOR;
        vector = Vector:malloc(E_VECTOR);
    } else {
        memcpy(v, Malloc_GetData(Alloc:vector, E_VECTOR_DATA), 0, E_VECTOR * 4, E_VECTOR);
        v[E_VECTOR_SIZE] += E_VECTOR;
        new 
            Alloc:a = malloc(v[E_VECTOR_SIZE], true);

        memcpy(Malloc_GetData(a, E_VECTOR_DATA), Malloc_GetData(v[E_VECTOR_DATA], E_VECTOR_DATA), 0, v[E_VECTOR_SIZE]*4, v[E_VECTOR_SIZE]);
        free(v[E_VECTOR_DATA]);
        v[E_VECTOR_DATA] = a;
    }

    new 
        value_ptr[E_VECTOR];

    ptr = value_ptr[E_VECTOR_DATA] = malloc(size == 0 ? 1 : size, true);
    value_ptr[E_VECTOR_SIZE] = size;

    memcpy(Malloc_GetData(Alloc:vector, E_VECTOR_DATA), v, 0, E_VECTOR * 4, E_VECTOR);
    memcpy(Malloc_GetData(v[E_VECTOR_DATA], E_VECTOR_DATA + v[E_VECTOR_SIZE] - E_VECTOR), value_ptr, 0, E_VECTOR * 4, E_VECTOR);
}

stock vector_push_back_value(&Vector:vector, value) {
    new 
        Alloc:ptr;

    vector_push_back_ptr(vector, ptr);
    Malloc_SetData(ptr, E_VECTOR_DATA, value);
}

stock vector_push_back_array(&Vector:vector, const array[], size = sizeof(array)) {
    new 
        Alloc:ptr;

    vector_push_back_ptr(vector, ptr, size);
    memcpy(Malloc_GetData(ptr, E_VECTOR_DATA), array, 0, size * 4, size);
}

stock bool:vector_free(&Vector:vector) {
    if(vector != VECTOR_NULL) {
        new 
            v[E_VECTOR];
    
        memcpy(v, Malloc_GetData(Alloc:vector, E_VECTOR_DATA), 0, E_VECTOR * 4, E_VECTOR);

        for(new i = 0; i < v[E_VECTOR_SIZE]; i += 2) {
            free(Alloc:Malloc_GetData(v[E_VECTOR_DATA], E_VECTOR_DATA + i));
        }

        free(v[E_VECTOR_DATA]);
        free(Alloc:vector);
        vector = VECTOR_NULL;

        return true;
    }

    return false;
}

stock bool:vector_remove(&Vector:vector, index) {
    if(vector != VECTOR_NULL) {
        new 
            v[E_VECTOR];

        memcpy(v, Malloc_GetData(Alloc:vector, 0), 0, E_VECTOR * 4, E_VECTOR);
        index *= E_VECTOR;

        if(0 <= index < v[E_VECTOR_SIZE]) {
            if(index == 0 && (v[E_VECTOR_SIZE] - E_VECTOR) == 0) {
                vector_free(vector);
            } else {
                free(Alloc:Malloc_GetData(v[E_VECTOR_DATA], E_VECTOR_DATA + index));

                for(new i = index, j = index + E_VECTOR; j < v[E_VECTOR_SIZE]; i += E_VECTOR) {
                    Malloc_SetData(v[E_VECTOR_DATA], E_VECTOR_DATA + i, Malloc_GetData(v[E_VECTOR_DATA], E_VECTOR_DATA + j));
                    Malloc_SetData(v[E_VECTOR_DATA], E_VECTOR_SIZE + i, Malloc_GetData(v[E_VECTOR_DATA], E_VECTOR_SIZE + j));
                    j += E_VECTOR;
                }

                v[E_VECTOR_SIZE] -= E_VECTOR;

                new 
                    Alloc:a = malloc(v[E_VECTOR_SIZE], true);

                memcpy(Malloc_GetData(a, E_VECTOR_DATA), Malloc_GetData(v[E_VECTOR_DATA], E_VECTOR_DATA), 0, (v[E_VECTOR_SIZE])*4, v[E_VECTOR_SIZE]);
                free(v[E_VECTOR_DATA]);
                v[E_VECTOR_DATA] = a;

                memcpy(Malloc_GetData(Alloc:vector, E_VECTOR_DATA), v, 0, E_VECTOR * 4, E_VECTOR);
            }

            return true;
        }
    }

    return false;
}

stock vector_get_value(Vector:vector, index) {
    if(vector != VECTOR_NULL) {
        new 
            Alloc:v_data = Alloc:Malloc_GetData(Alloc:vector, E_VECTOR_DATA),
            v_size = Malloc_GetData(Alloc:vector, E_VECTOR_SIZE);

        index *= 2;

        if(0 <= index < v_size) {
            return Malloc_GetData(Alloc:Malloc_GetData(v_data, E_VECTOR_DATA + index), E_VECTOR_DATA);
        }
    }

    return INVALID_VECTOR_INDEX;
}

stock vector_get_array(Vector:vector, index, array[], size = sizeof(array)) {
    if(vector != VECTOR_NULL) {
        new 
            Alloc:v_data = Alloc:Malloc_GetData(Alloc:vector, E_VECTOR_DATA),
            v_size = Malloc_GetData(Alloc:vector, E_VECTOR_SIZE);

        index *= 2;

        if(0 <= index < v_size) {
            new 
                Alloc:ptr_data = Alloc:Malloc_GetData(v_data, E_VECTOR_DATA + index),
                ptr_size = Malloc_GetData(v_data, E_VECTOR_SIZE + index);

            new 
                n_sz = ((size > ptr_size) ? ptr_size : size);

            memcpy(array, Malloc_GetData(ptr_data, E_VECTOR_DATA), 0, n_sz * 4, n_sz);

            return n_sz;
        }
    }

    return INVALID_VECTOR_INDEX;
}

stock bool:vector_set_value(Vector:vector, index, value) {
    if(vector != VECTOR_NULL) {
        new 
            v[E_VECTOR];

        memcpy(v, Malloc_GetData(Alloc:vector, E_VECTOR_DATA), 0, E_VECTOR * 4, E_VECTOR);
        index *= 2;

        if(0 <= index < v[E_VECTOR_SIZE]) {
            Malloc_SetData(Alloc:Malloc_GetData(v[E_VECTOR_DATA], E_VECTOR_DATA + index), E_VECTOR_DATA, value);

            return true;
        }
    }

    return false;
}

stock bool:vector_set_array(Vector:vector, index, array[], size = sizeof(array)) {
    if(vector != VECTOR_NULL) {
        new 
            v[E_VECTOR];

        memcpy(v, Malloc_GetData(Alloc:vector, E_VECTOR_DATA), 0, E_VECTOR * 4, E_VECTOR);
        index *= 2;

        if(0 <= index < v[E_VECTOR_SIZE]) {
            new 
                ptr[E_VECTOR];

            memcpy(ptr, Malloc_GetData(v[E_VECTOR_DATA], E_VECTOR_DATA + index), 0, E_VECTOR * 4, E_VECTOR);
            if(size != ptr[E_VECTOR_SIZE]) {
                free(ptr[E_VECTOR_DATA]);
                ptr[E_VECTOR_DATA] = malloc(size, true);
                ptr[E_VECTOR_SIZE] = size;
            }

            memcpy(Malloc_GetData(ptr[E_VECTOR_DATA], E_VECTOR_DATA), array, 0, size * 4, size);
            memcpy(Malloc_GetData(v[E_VECTOR_DATA], E_VECTOR_DATA + index), ptr, 0, E_VECTOR * 4, E_VECTOR);

            return true;
        }
    }

    return false;
}

stock bool:vector_is_storing_array(Vector:vector) {
    if(vector != VECTOR_NULL) {
        return Malloc_GetData(Alloc:Malloc_GetData(Alloc:vector, E_VECTOR_DATA), E_VECTOR_SIZE) != 0;
    }

    return false;
}

stock vector_get_array_size(Vector:vector, index) {
    if(vector != VECTOR_NULL) {
        new 
            v[E_VECTOR];

        memcpy(v, Malloc_GetData(Alloc:vector, E_VECTOR_DATA), 0, E_VECTOR * 4, E_VECTOR);
        index *= 2;

        if(0 <= index < v[E_VECTOR_SIZE]) {
            return Malloc_GetData(v[E_VECTOR_DATA], E_VECTOR_SIZE);
        }
    }

    return 0;
}

stock vector_get_size(Vector:vector) {
    if(vector != VECTOR_NULL) {
        return Malloc_GetData(Alloc:vector, E_VECTOR_SIZE) / E_VECTOR;
    }

    return 0;
}

static stock vector_sort_values_helper(Alloc:v_data, first, last) {
    if(first < last) {
        new 
            splitpoint = vector_sort_values_partition(v_data, first, last);

        vector_sort_values_helper(v_data, first, splitpoint - E_VECTOR);
        vector_sort_values_helper(v_data, splitpoint + E_VECTOR, last);
    }
}

static stock vector_sort_values_partition(Alloc:v_data, first, last) {
    new 
        pvt_value = Malloc_GetData(Alloc:Malloc_GetData(v_data, E_VECTOR_DATA + first), E_VECTOR_DATA),
        left_mark = first + E_VECTOR,
        right_mark = last,
        bool:done = false,
        tmp;

    while(!done) {
        while(left_mark <= right_mark && Malloc_GetData(Alloc:Malloc_GetData(v_data, E_VECTOR_DATA + left_mark), E_VECTOR_DATA) <= pvt_value) {
            left_mark += E_VECTOR;
        }
        while(Malloc_GetData(Alloc:Malloc_GetData(v_data, E_VECTOR_DATA + right_mark), E_VECTOR_DATA) >= pvt_value && right_mark >= left_mark) {
            right_mark -= E_VECTOR;
        }

        if(right_mark < left_mark) {
            done = true;
        } else {
            tmp = Malloc_GetData(v_data, E_VECTOR_DATA + left_mark);
            Malloc_SetData(v_data, E_VECTOR_DATA + left_mark, Malloc_GetData(v_data, E_VECTOR_DATA + right_mark));
            Malloc_SetData(v_data, E_VECTOR_DATA + right_mark, tmp);
        }
    }

    tmp = Malloc_GetData(v_data, E_VECTOR_DATA + first);
    Malloc_SetData(v_data, E_VECTOR_DATA + first, Malloc_GetData(v_data, E_VECTOR_DATA + right_mark));
    Malloc_SetData(v_data, E_VECTOR_DATA + right_mark, tmp);

    return right_mark;
}

stock bool:vector_sort(Vector:vector, order=E_VECTOR_SORT_ASC) {
    if(vector != VECTOR_NULL) {
        new 
            Alloc:v_data = Alloc:Malloc_GetData(Alloc:vector, E_VECTOR_DATA),
            v_size = Malloc_GetData(Alloc:vector, E_VECTOR_SIZE);

        if(!vector_is_storing_array(vector)) {
            vector_sort_values_helper(v_data, 0, v_size - E_VECTOR);

            if(order == E_VECTOR_SORT_DESC) {
                for(new i = 0, j = v_size - 2; i < j; i += 2, j -= 2) {
                    new 
                        tmp = Malloc_GetData(Alloc:Malloc_GetData(v_data, E_VECTOR_DATA + i), E_VECTOR_DATA);

                    Malloc_SetData(Alloc:Malloc_GetData(v_data, E_VECTOR_DATA + i), E_VECTOR_DATA, Malloc_GetData(Alloc:Malloc_GetData(v_data, E_VECTOR_DATA + j), E_VECTOR_DATA));
                    Malloc_SetData(Alloc:Malloc_GetData(v_data, E_VECTOR_DATA + j), E_VECTOR_DATA, tmp);
                }
            }

            return true;
        }
    }

    return false;
}