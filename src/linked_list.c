/* linked_list.c */
#include <stddef.h>
#include "ministream.h"
#include "linked_list.h"

/* Linked list uzerinde lineer arama - O(n)
   bas: listenin ilk dugumu
   id:  aranacak sarki id'si */
Sarki* sarki_ara_liste(Sarki* bas, int id) {
    Sarki* curr = bas;
    while (curr != NULL) {
        if (curr->id == id) return curr;
        curr = curr->sonraki;
    }
    return NULL;
}
