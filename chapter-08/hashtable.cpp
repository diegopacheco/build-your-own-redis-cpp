#include <assert.h>
#include <stdlib.h> // calloc(), free()
#include <hashtable.h>

// n must be a power of 2
static void h_init(HTab *htab, size_t n){
    assert(n > 0 && ((n -1) & n) == 0);
    htab->tab = (HNode **)calloc(n, sizeof(HNode *));
    htab->mask = n - 1;
    htab->size = 0;
}

// hashtable insertion
static void h_inser(HTab *htab, HNode *node){
    size_t pos = node->hcode & htab->mask;
    HNode *next = htab->tab[pos];
    node->next = next;
    htab->tab[pos]=node;
    htab->size++;
}

// hastable look up subroutine.
// Pay attention to the return value. It returns the addess of
// the parenbt pointer that owns the target node.
// which can be used to delete the target node.
static HNode **h_lookup(HTab *htab, HNode *key, bool (*eq)(HNode *,HNode *)){
    if (!htab->tab){
        return NULL;
    }

    size_t pos = key->hcode & htab->mask;
    HNode **from = &htab->tab[pos]; // incoming pointer to the target
    for (HNode *cur; (cur = *from) != NULL; from = &cur->next){
        if (cur->hcode == key->hcode && eq(cur, key)){
            return from; // may be a node, may be a slot
        }
    }
    return NULL;
}

