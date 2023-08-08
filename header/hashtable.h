#include <stdint.h>
#include "./utils.h"

#ifndef HASH_H
#define HASH_H

// Node for hashtable.
struct HNode
{
    HNode *next = NULL;
    uint64_t hcode = 0;
};

// Fixed-size hashtable.
struct HTab {
    HNode **table = NULL;
    size_t mask = 0;
    size_t size = 0;
};


void h_init(HTab *htab, size_t n);
void h_insert(HTab *htab, HNode *node);
HNode **h_lookup(HTab *htab, HNode *key, bool (*cmp)(HNode*, HNode*));
HNode *h_detach(HTab *htab, HNode **from);

#endif