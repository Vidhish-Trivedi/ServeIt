#include "./../header/hashtable.h"

// Size of hashtable is asserted to be power of 2, makes indexing easier.
void h_init(HTab *htab, size_t n) {
    assert(n > 0 && ((n - 1) & n) == 0);
    htab->table = (HNode**)calloc(sizeof(HNode*), n);
    htab->mask = n - 1;
    htab->size = 0;
}

// Hashtable insert.
void h_insert(HTab *htab, HNode *node) {
    size_t pos = node->hcode & htab->mask;
    HNode *next = htab->table[pos];
    node->next = next;
    htab->table[pos] = node;
    htab->size++;
}

// Hashtable lookup is a simple list traversal.
// Returns the address of the parent pointer that owns the target node.
// (Returns address of a row of nodes containing the target node).
HNode **h_lookup(HTab *htab, HNode *key, bool (*cmp)(HNode*, HNode*)) {
    if(!htab->table) {
        return (NULL);
    }

    size_t pos = key->hcode & htab->mask;
    HNode **from = &htab->table[pos];

    while(*from) {
        if(cmp(*from, key)) {
            return(from);
        }
        from = &((*from)->next);
    }
    return(NULL);
}

// Delete a node from the hashtable.
HNode *h_detach(HTab *htab, HNode **from) {
    HNode *node = *from;
    *from = (*from)->next;
    htab->size--;
    return(node);
}
