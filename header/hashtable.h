#include <stdint.h>
#include <string>
#include "./utils.h"

#ifndef HASH_H
#define HASH_H

const size_t k_resizing_work = 128;
const size_t k_max_load_factor = 8;

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

// Hashtable interface, 2 hashtables for progressive resizing.
struct HMap {
    HTab ht1;
    HTab ht2;
    size_t resizing_pos = 0;
};

// Struct for data payload.
struct entry {
    struct HNode node;
    std::string key;
    std::string val;
};


void hm_help_resize(HMap *hmap);
void hm_start_resize(HMap *hmap);

void h_init(HTab *htab, size_t n);
void h_insert(HTab *htab, HNode *node);
HNode **h_lookup(HTab *htab, HNode *key, bool (*cmp)(HNode*, HNode*));
HNode *h_detach(HTab *htab, HNode **from);
HNode *hm_lookup(HMap *hmap, HNode *key, bool (*cmp)(HNode *, HNode *));
void hm_insert(HMap *hmap, HNode *node);
HNode *hm_del(HMap *hmap, HNode *key, bool (*cmp)(HNode *, HNode *));

#endif