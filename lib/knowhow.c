#ifndef knowhow_c
#define knowhow_c
#include <interface.c>
#include <public.c>
#include <protected.c>
#include <private.c>
#include <stdlib.h>
#include <delete.c>

void *lookup_rec(ROOT, u_int32_t k) {
    int i = 0;
    while (i < p_root->n && k > p_root->p_keys[i].key) {
        i++;
    }
    if (i < p_root->n && p_root->p_keys[i].key == k) {
        return p_root->p_keys[i].p_context;
    }
    if (p_root->leaf) {
        return NULL;
    }
    return lookup_rec(p_root->pp_child[i], k);
}

void* lookup(TREE, const u_int32_t key)
{
    if(NULL != p_tree && NULL != p_tree->data)
        return lookup_rec(p_tree->data, key);
    return NULL;
}

void split_child(ROOT, int i) {
    int t = p_root->t;
    node_t *z = create_node(t, p_root->pp_child[i]->leaf);
    node_t *y = p_root->pp_child[i];
    z->n = t - 1;
    for (int j = 0; j < t - 1; j++) {
        z->p_keys[j] = y->p_keys[j + t];
    }
    if (!y->leaf) {
        for (int j = 0; j < t; j++) {
            z->pp_child[j] = y->pp_child[j + t];
        }
    }
    y->n = t - 1;
    for (int j = p_root->n; j >= i + 1; j--) {
        p_root->pp_child[j + 1] = p_root->pp_child[j];
    }
    p_root->pp_child[i + 1] = z;
    for (int j = p_root->n - 1; j >= i; j--) {
        p_root->p_keys[j + 1] = p_root->p_keys[j];
    }
    p_root->p_keys[i] = y->p_keys[t - 1];
    p_root->n++;
}

void insert_non_full(ROOT, void_ref_t k) {
    int i = p_root->n - 1;
    if (p_root->leaf) {
        while (i >= 0 && p_root->p_keys[i].key > k.key) {
            p_root->p_keys[i + 1] = p_root->p_keys[i];
            i--;
        }
        p_root->p_keys[i + 1] = k;
        p_root->n++;
    } else {
        while (i >= 0 && p_root->p_keys[i].key > k.key) {
            i--;
        }
        i++;
        if (p_root->pp_child[i]->n == 2 * p_root->t - 1) {
            split_child(p_root, i);
            if (p_root->p_keys[i].key < k.key) {
                i++;
            }
        }
        insert_non_full(p_root->pp_child[i], k);
    }
}


bool insert_rec(PROOT, void_ref_t k, u_int32_t t) {
    if (*pp_root == NULL) {
        *pp_root = create_node(t, 1);
        (*pp_root)->p_keys[0] = k;
        (*pp_root)->n = 1;
    } else {
        if ((*pp_root)->n == 2 * t - 1) {
            node_t *s = create_node(t, 0);
            s->pp_child[0] = *pp_root;
            split_child(s, 0);
            int i = 0;
            if (s->p_keys[0].key < k.key) {
                i++;
            }
            insert_non_full(s->pp_child[i], k);
            *pp_root = s;
        } else {
            insert_non_full(*pp_root, k);
        }
    }
    return true;
}

bool insert(TREE, void_ref_t ref)
{
    if(NULL != p_tree)
        return insert_rec(&p_tree->data, ref, p_tree->t);
    return false;
}




#endif
