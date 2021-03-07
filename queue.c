#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
queue_t *q_new()
{
    queue_t *q = malloc(sizeof(queue_t));

    if (!q) {
        return NULL;
    }

    // init the q
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
    return q;
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{
    if (!q) {
        return;
    }

    list_ele_t *current = q->head;
    list_ele_t *tmp = NULL;
    while (current) {
        tmp = current;
        current = current->next;
        free(tmp->value);
        free(tmp);
    }

    /* Free queue structure */
    free(q);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(queue_t *q, char *s)
{
    if (!q) {
        return false;
    }

    list_ele_t *newh;
    size_t len = strlen(s);

    newh = malloc(sizeof(list_ele_t));
    if (!newh) {
        return false;
    }

    newh->value = malloc(len + 1);
    if (!newh->value) {
        free(newh);
        return false;
    }

    strncpy(newh->value, s, len);
    newh->value[len] = '\0';

    if (!q->head) {
        // if q->head is NULL, then attach both head and tail to the new_head
        // maybe there's a better approche then this...
        q->tail = newh;
    }
    newh->next = q->head;
    q->head = newh;

    q->size++;
    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(queue_t *q, char *s)
{
    if (!q) {
        return false;
    }

    list_ele_t *newt;

    newt = malloc(sizeof(list_ele_t));
    if (!newt) {
        return false;
    }

    size_t len = strlen(s);
    newt->value = malloc(len + 1);
    if (!newt->value) {
        free(newt);
        return false;
    }
    strncpy(newt->value, s, len);
    newt->value[len] = '\0';
    newt->next = NULL;


    if (!q->tail) {
        q->head = newt;
        q->tail = newt;
    } else {
        q->tail->next = newt;
        q->tail = newt;
    }

    q->size++;

    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return true if successful.
 * Return false if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 * The space used by the list element and the string should be freed.
 */
bool q_remove_head(queue_t *q, char *sp, size_t bufsize)
{
    if (!q || !q->head) {
        return false;
    }

    // Copy the str form q->head->value to sp if sp is non-NULL
    char *str = q->head->value;
    if (str && sp) {
        strncpy(sp, str, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    // Remove the node
    list_ele_t *tmp;
    tmp = q->head;
    q->head = q->head->next;
    q->size--;

    free(tmp->value);
    free(tmp);

    return true;
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
    if (!q) {
        return 0;
    }

    return q->size;
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(queue_t *q)
{
    if (!q || !q->head) {
        return;
    }

    list_ele_t *next = NULL;
    list_ele_t *cur = q->head;
    q->tail = q->head;

    // q->head is the "previous"
    q->head = NULL;
    while (cur) {
        next = cur->next;
        cur->next = q->head;
        q->head = cur;
        cur = next;
    }
}

list_ele_t *merge(list_ele_t *l_head, list_ele_t *r_head)
{
    list_ele_t result;  // the first element of the result lis  (but we actually
                        // only want the result.next)
    list_ele_t *result_tail = &result;

    while (l_head && r_head) {
        list_ele_t **target;
        target =
            (strcmp(l_head->value, r_head->value) <= 0) ? &l_head : &r_head;

        result_tail->next = *target;
        result_tail = result_tail->next;

        *target = (*target)->next;  // change the pos of l_head or r_head
    }

    // if l_head is not NULL then concat the whole l list to the result
    if (l_head) {
        result_tail->next = l_head;
    } else if (r_head) {
        result_tail->next = r_head;
    }

    return result.next;
}


list_ele_t *merge_sort(list_ele_t *head, int size)
{
    if (size <= 1) {
        return head;
    }

    int half = size >> 1;
    list_ele_t *l_head = head, *r_head = head;
    list_ele_t *l_tail = NULL;

    for (int i = 0; i < half - 1; i++) {
        r_head = r_head->next;
    }
    l_tail = r_head;
    r_head = r_head->next;
    l_tail->next = NULL;

    list_ele_t *l_result = merge_sort(l_head, half);
    list_ele_t *r_result = merge_sort(r_head, size - half);

    return merge(l_result, r_result);
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(queue_t *q)
{
    if (!q || !q->head || q_size(q) <= 1) {
        return;
    }

    q->head = merge_sort(q->head, q->size);

    // May be change the merge_sort to the form: `merge_sort(**head, **tail,
    // size)`, to reduse the cost of finding the q->tail
    q->tail = q->head;
    while (q->tail->next) {
        q->tail = q->tail->next;
    }
}
