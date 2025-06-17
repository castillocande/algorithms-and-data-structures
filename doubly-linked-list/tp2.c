#include "tp2.h"
#include <stdlib.h>
#include <stdbool.h>

struct node;
typedef struct node node_t;

struct node {
    void* value;
    node_t* next;
    node_t* prev;
};

struct list {
    node_t* head;
    node_t* tail;
    size_t size;
};

struct list_iter {
    list_t* list;
    node_t* curr;
};

list_t *list_new() {
    list_t *list = (list_t *) malloc(sizeof(list_t));
    if (list == NULL) {
        return NULL;
    }
    list->head = NULL; 
    list->tail = NULL;
    list->size = 0;
    return list; 
}

size_t list_length(const list_t *list) { return list->size; }

bool list_is_empty(const list_t *list) { return list->size == 0; }

bool list_insert_head(list_t *list, void *value) {
    node_t *node = (node_t *) malloc(sizeof(node_t));
    if (node == NULL) {
        return false;
    }
    node->value = value;
    node->prev = NULL;
    node->next = list->head; // si la lista está vacía, queda en NULL
    if (list_is_empty(list)) {
        list->tail = node;         
    } else {
        list->head->prev = node; 
    }
    list->head = node;
    list->size++;
    return true;
}

bool list_insert_tail(list_t *list, void *value){
    node_t *node = (node_t *) malloc(sizeof(node_t));
    if (node == NULL) {
        return false;
    }
    node->value = value;
    node->next = NULL;
    node->prev = list->tail;
    if (list_is_empty(list)) {
        list->head = node;
    } else {
        list->tail->next = node;
    }
    list->tail = node;
    list->size++;
    return true;

}

void *list_peek_head(const list_t *list) { 
    if (list_is_empty(list)) { 
        return NULL; 
    }    
    return list->head->value;
}

void *list_peek_tail(const list_t *list) {
    if (list_is_empty(list)) { 
        return NULL; 
    }    
    return list->tail->value;
}

void *list_pop_head(list_t *list) { 
    if (list_is_empty(list)) {
        return NULL;
    }
    node_t *aux = list->head; 
    list->head = aux->next; // está seteado en NULL si la lista tiene un solo elemento
    void *value = aux->value; 
    free(aux);
    if (list->size == 1) { 
        list->tail = NULL; 
    } else {
        list->head->prev = NULL;  
    }
    list->size--;
    return value;
}

void *list_pop_tail(list_t *list) {
    if (list_is_empty(list)) {
        return NULL;
    }
    node_t *aux = list->tail; 
    void *value = aux->value;
    list->tail = aux->prev;
    free(aux);
    if (list->size == 1) {
        list->head = NULL;
    } else {
        list->tail->next = NULL;
    }
    list->size--;
   return value;
}

void list_destroy(list_t *list, void destroy_value(void *)) { 
    bool is_valid = destroy_value != NULL ? true : false;
    while (!list_is_empty(list)) {
        void *value = list_pop_head(list);
        if (is_valid && value != NULL) { 
            destroy_value(value);
        }
    }
    free(list); 
}

list_iter_t *list_iter_create_head(list_t *list) { 
    list_iter_t *iter = (list_iter_t *) malloc(sizeof(list_iter_t));
    if (iter == NULL) {
        return NULL;
    }
    iter->list = list;
    iter->curr = list->head; 
    return iter;
}

list_iter_t *list_iter_create_tail(list_t *list) {
    list_iter_t *iter = (list_iter_t *) malloc(sizeof(list_iter_t));
    if (iter == NULL) {
        return NULL;
    }
    iter->list = list;
    iter->curr = list->tail; 
    return iter;
}

bool list_iter_forward(list_iter_t *iter) {
    if (list_iter_at_last(iter)) {
        return false;
    }
    iter->curr = iter->curr->next;
    return true;
}

bool list_iter_backward(list_iter_t *iter) {
    if (list_iter_at_first(iter)) {
        return false;
    }
    iter->curr = iter->curr->prev;
    return true;
}

void *list_iter_peek_current(const list_iter_t *iter) {
    if (list_is_empty(iter->list)) {
        return NULL;
    }
    return iter->curr->value;
}

bool list_iter_at_last(const list_iter_t *iter) {
    if (list_is_empty(iter->list)) {
        return true;
    }
    return iter->curr == iter->list->tail; 
}

bool list_iter_at_first(const list_iter_t *iter) {
    if (list_is_empty(iter->list)) { 
        return true;
    }
    return iter->curr == iter->list->head;
}

void list_iter_destroy(list_iter_t *iter) { free(iter); } 

bool list_iter_insert_after(list_iter_t *iter, void *value) {
    node_t *node = (node_t *) malloc(sizeof(node_t));
    if (node == NULL) {
        return false;
    }
    if (list_iter_at_last(iter)) {
        iter->list->tail = node;
        if (list_is_empty(iter->list)) { 
            iter->curr = node;
            iter->list->head = node;
        }
    }
    node->next = iter->curr->next;
    node->prev = iter->curr;
    node->value = value;

    iter->curr->next = node;
    iter->list->size++;
    return true;
}

bool list_iter_insert_before(list_iter_t *iter, void *value) { 
    node_t *node = (node_t *) malloc(sizeof(node_t));
    if (node == NULL) {
        return false;
    }
    if (list_iter_at_first(iter)) {
        iter->list->head = node;
        if (list_is_empty(iter->list)) {
            iter->curr = node;
            iter->list->tail = node;
        } 
    } else {
        iter->curr->prev->next = node;
    }
    node->next = iter->curr;
    node->prev = iter->curr->prev; // si iter->curr está en el primero, iter->curr->prev sería NULL
    node->value = value;
    
    iter->curr->prev = node;
    iter->list->size++;
    return true;
}

void *list_iter_delete(list_iter_t *iter) { 
    if (list_is_empty(iter->list)) {
        return NULL;
    }
    void *value;
    if (list_iter_at_first(iter)) {
        iter->curr = iter->curr->next;  // si usara list_iter_forward en una lista de un elemento, no se acualizaría iter->curr
        value = list_pop_head(iter->list);
    } else if (list_iter_at_last(iter)) {
        iter->curr = iter->curr->prev; 
        value = list_pop_tail(iter->list);
    } else {
        node_t *aux = iter->curr;
        value = list_iter_peek_current(iter);
        iter->curr->prev->next = aux->next;
        iter->curr->next->prev = aux->prev;
        list_iter_forward(iter);
        free(aux);
        iter->list->size--;
    }
    return value;
}