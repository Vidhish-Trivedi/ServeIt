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

// Function to help resize HMap.
void hm_help_resize(HMap *hmap) {
    if(hmap->ht2.table == NULL) {
        return;
    }

    size_t n_work = 0;
    while(n_work < k_resizing_work && hmap->ht2.size > 0) {
        // Find nodes in ht2 and move them to ht1.
        HNode **from = &hmap->ht2.table[hmap->resizing_pos];
        
        // Not Found.
        if(!*from) {
            hmap->resizing_pos++;
            continue;
        }

        // Found.
        h_insert(&hmap->ht1, h_detach(&hmap->ht2, from));
        n_work++;
    }

    // Cleanup.
    if(hmap->ht2.size == 0) {
        free(hmap->ht2.table);
        hmap->ht2 = HTab{};
    }
}


// The lookup with resizing.
HNode *hm_lookup(HMap *hmap, HNode *key, bool (*cmp)(HNode *, HNode *)) {
    hm_help_resize(hmap);
    
    // look in first table.
    HNode **from = h_lookup(&hmap->ht1, key, cmp);
    
    // look in second table.
    if(!from) {
        from = h_lookup(&hmap->ht2, key, cmp);
    }

    return (from ? *from : NULL);
}

// Function to initialize a larger hashtable for resizing.
void hm_start_resize(HMap *hmap) {
    assert(hmap->ht2.table == NULL);
    // swap current table.
    hmap->ht2 = hmap->ht1;

    // create a bigger table.
    h_init(&hmap->ht1, (hmap->ht1.mask + 1)*2);
    hmap->resizing_pos = 0;
}

// Insertion with resizing, will trigger resizing if the table becomes too full.
void hm_insert(HMap *hmap, HNode *node) {
    if(!hmap->ht1.table) {
        h_init(&hmap->ht1, 4);
    }
    h_insert(&hmap->ht1, node);

    if(!hmap->ht2.table) {
        // Check if resizing is required.
        size_t load_factor = hmap->ht1.size / (hmap->ht1.mask + 1);
        if(load_factor >= k_max_load_factor) {
            hm_start_resize(hmap);
        }
    }
    hm_help_resize(hmap);
}

// Deleting a key from HMap.
HNode *hm_del(HMap *hmap, HNode *key, bool (*cmp)(HNode *, HNode *)) {
    hm_help_resize(hmap);
    
    // Look for key in ht1.
    HNode **from = h_lookup(&hmap->ht1, key, cmp);
    if(from) {
        return (h_detach(&hmap->ht1, from));
    }

    // Look for key in ht2.
    from = h_lookup(&hmap->ht2, key, cmp);
    if(from) {
        return (h_detach(&hmap->ht2, from));
    }

    return(NULL);
}

void hm_destroy(HMap *hmap) {
    assert(hmap->ht1.size + hmap->ht2.size == 0);
    free(hmap->ht1.table);
    free(hmap->ht2.table);
    *hmap = HMap{};
}
