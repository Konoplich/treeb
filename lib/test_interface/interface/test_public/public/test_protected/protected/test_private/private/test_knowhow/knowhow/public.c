#ifndef public_c
#define public_c
#include <interface.c>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

bool new(PTREE, u_int32_t M)
{
    p("new %u\n", M);
    *pp_tree = malloc(sizeof(treeb_t));
    if(NULL !=*pp_tree)
    {
        memset(*pp_tree, 0, sizeof(treeb_t));
        (*pp_tree)->max_children = M; //max children
        (*pp_tree)->max_keys = M-1; //max children
        (*pp_tree)->min_keys = (int)(ceil(M/2.0))-1; //max children
        return true;
    }
    return false;
}

void free_node(PROOT)
{
    p("free node ");
	if (NULL != *pp_root) {

        p(" %p n:%d\n", *pp_root, (*pp_root)->cnt_child);
        for(u_int32_t i=0;i<(*pp_root)->cnt_child;i++)
        {
            p("i %p\n", (*pp_root)->child[i]);
			free_node(&(*pp_root)->child[i]);
        }
        SafeDelete((*pp_root)->child);
        SafeDelete((*pp_root)->key);
        p("del pp_root: %p %d\n", (*pp_root), (*pp_root)->cnt_child);
        SafeDelete(*pp_root);
        *pp_root = NULL;
        p("free ok\n");
	}
}

void destructor(PTREE)
{
    p("destructor enter\n");
    p("destructor: %p p_root: %p, data: %p\n", pp_tree, *pp_tree, (*pp_tree)->data);
    free_node(&(*pp_tree)->data);
    SafeDelete(*pp_tree);
    *pp_tree = NULL;
}

node_t *create_node(const TREE, void_ref_t key) {
    p("max_keys %d max_children %d min_keys %d\n", p_tree->max_keys, p_tree->max_children, p_tree->min_keys);
    node_t *node = (node_t *)malloc(sizeof(node_t));
    node->leaf = false;
    node->key = (void_ref_t *)malloc((p_tree->max_keys+1) * sizeof(void_ref_t));
    //memset(node->key, 0, (p_tree->max_keys+1) * sizeof(void_ref_t));
    node->child = (node_t **)malloc((p_tree->max_children+1) * sizeof(node_t*));
    //memset(node->child, 0, (p_tree->max_children+1) * sizeof(node_t*));
    node->cnt_key = 1;
    node->cnt_child = 0;
    node->key[0] = key;
    return node;
}

void print_rec(ROOT, u_int32_t prefix) 
{
    if (p_root != NULL) {
        int i;
        
        p("%p %d-%d ",p_root, p_root->cnt_key, p_root->cnt_child);
        for (i = 0; i < p_root->cnt_key; i++) {
            for(int j=0; j<prefix;j++)
                printf("-");
            printf(" ");

            p("%b %d-%s ",p_root->leaf,  p_root->key[i].key, (char*)p_root->key[i].p_context);
            printf("%d-%s\n",  p_root->key[i].key, (char*)p_root->key[i].p_context);
        }
        for (i = 0; i < p_root->cnt_child; i++) {
            print_rec(p_root->child[i], prefix+3);
        }
    }
}

void print(TREE)
{
    if(NULL != p_tree && NULL !=p_tree->data)
    {
        p("print %p->%p\n", p_tree, p_tree->data);
	    print_rec(p_tree->data, 3);
    }
}

#endif
