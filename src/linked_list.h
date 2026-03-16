/* linked_list.h */
#ifndef LINKED_LIST_H
#define LINKED_LIST_H

/* Forward declaration - tam tanimim ministream.h'da */
typedef struct Sarki Sarki;

/* Linked list uzerinden lineer arama - O(n) */
Sarki* sarki_ara_liste(Sarki* bas, int id);

#endif /* LINKED_LIST_H */
