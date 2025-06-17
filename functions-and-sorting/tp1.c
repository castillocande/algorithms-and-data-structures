#include "tp1.h"
#include <stdlib.h>
#include <math.h>

bool is_prime(int x) { 
    if (x <= 1) { 
        return false; 
    }
    int sqrt_x = (int) sqrt(x);
    for (size_t i = 2; i <= sqrt_x; i++) { 
        if (x % i == 0) { 
            return false; 
        }
    }
    return true;
}

int storage_capacity(float d, float v) { // se asumen D y V valores positivos
    if (v == 0) { 
        return 0;
    }
    return (int) (d / v);
}

void swap(int *x, int *y) {
    int aux = *x;
    *x = *y;
    *y = aux;
}

int array_max(const int *array, int length) {
    int max = array[0]; // se asume que el arreglo tiene por lo menos un elemento
    // si el arreglo no fuera válido y devolviéramos un int como flag, no habría forma de distinguirlo de un int que represente al max
    for (size_t i = 1; i < length; i++) {
        if (max < array[i]) {
            max = array[i];
        }
    }
    return max;
} 

void array_map(int *array, int length, int f(int)) {
    if (f == NULL) { 
        return;
    }
    for (size_t i = 0; i < length; i++) {
        array[i] = f(array[i]); 
    }
}

int *copy_array(const int *array, int length) { 
    if (array == NULL) {
        return NULL;
    }

    int *array_copy = (int *) malloc(length * sizeof(int));
    if (array_copy == NULL) {
        return NULL; 
    }

    for (size_t i = 0; i < length; i++) {
        array_copy[i] = array[i];
    }
    return array_copy;
}

int **copy_array_of_arrays(const int **array_of_arrays, const int *array_lenghts, int array_amount) { 
    if (array_of_arrays == NULL) {
        return NULL; 
    }
    
    int **array_of_arrays_copy = (int **) malloc(array_amount * sizeof(int *)); 
    if (array_of_arrays_copy == NULL) {
        return NULL; 
    }

    for (size_t i = 0; i < array_amount; i++) {
        if (array_of_arrays[i] == NULL) {
            array_of_arrays_copy[i] = NULL;
        } else {
            array_of_arrays_copy[i] = copy_array(array_of_arrays[i], array_lenghts[i]);
            if (array_of_arrays_copy[i] == NULL) {
                for (size_t j = 0; j < i; j++) { 
                    if (array_of_arrays_copy[j]) { 
                        free(array_of_arrays_copy[j]); // falló una de las asignaciones de memoria para un arreglo, entonces, liberamos los anteriores
                        // no se hace uso de la función free_array_of_arrays porque tendríamos que hacer una copia de array_lengths solo para el caso en que falle
                    }
                }
                free(array_of_arrays_copy); 
                return NULL;
            }
        }
        
    }
    return array_of_arrays_copy;
}

void free_array_of_arrays(int **array_of_arrays, int *array_lenghts, int array_amount) {
    for (size_t i = 0; i < array_amount; i++) {
        free(array_of_arrays[i]);
    }
    free(array_of_arrays); 
    free(array_lenghts); 
}

void bubble_sort(int *array, int length) {
    if (array == NULL) {
        return;
    }

    for (size_t i = 0; i < length - 1; i++) { 
        for (size_t j = 0; j < length - i - 1; j++) {
            if (array[j] > array[j + 1]) {
                swap(&array[j], &array[j + 1]);
            }
        }
    }
}

bool array_equal(const int *array1, int length1, const int *array2, int length2) {
    if (length1 != length2) {
        return false;
    }
    if (array1 == NULL || array2 == NULL) {
        return array1 == array2; 
    }

    for (size_t i = 0; i < length1; i++) {
        if (array1[i] != array2[i]) {
            return false;
        }
    }
    return true;
}

bool integer_anagrams(const int *array1, int length1,
                      const int *array2, int length2) {
    if (length1 != length2 || array1 == NULL || array2 == NULL) { 
        return false;
    }

    int *array_copy1 = copy_array(array1, length1); 
    if (array_copy1 == NULL) {
        return false; 
    }
    int *array_copy2 = copy_array(array2, length2);
    if (array_copy2 == NULL) {
        free(array_copy1);
        return false;
    }

    bubble_sort(array_copy1, length1);
    bubble_sort(array_copy2, length2);

    bool result = array_equal(array_copy1, length1, array_copy2, length2);

    free(array_copy1);
    free(array_copy2);
    
    return result;
}