#ifndef public_c
#define public_c
#include <interface.c>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

bool new(PTREE, u_int32_t min_degree)
{
    p("new %u\n", min_degree);
    *pp_tree = malloc(sizeof(treeb_t));
    if(NULL !=*pp_tree)
    {
        memset(*pp_tree, 0, sizeof(treeb_t));
        (*pp_tree)->t = min_degree;
        return true;
    }
    return false;
}

void free_node(TREE, PROOT)
{
    p("free node ");
	if (NULL != *pp_root) {

        p(" %p n:%d\n", *pp_root, (*pp_root)->n);
        for(u_int32_t i=0;i<p_tree->t;i++)
        {
            p("i %p\n", (*pp_root)->pp_child[i]);
			free_node(p_tree, &(*pp_root)->pp_child[i]);
        }
        SafeDelete((*pp_root)->pp_child);
        SafeDelete((*pp_root)->p_keys);
        p("del pp_root: %p %d\n", (*pp_root), (*pp_root)->n);
        SafeDelete(*pp_root);
        *pp_root = NULL;
        p("free ok\n");
	}
}

void destructor(PTREE)
{
    p("destructor enter\n");
    p("destructor: %p p_root: %p, data: %p\n", pp_tree, *pp_tree, (*pp_tree)->data);
    free_node(*pp_tree, &(*pp_tree)->data);
    SafeDelete(*pp_tree);
    *pp_tree = NULL;
}

node_t *create_node(u_int32_t t, u_int32_t leaf) {
    node_t *node = (node_t *)malloc(sizeof(node_t));
    node->t = t;
    node->leaf = leaf;
    node->p_keys = (void_ref_t *)malloc((2 * t - 1) * sizeof(void_ref_t));
    node->pp_child = (node_t **)malloc(2 * t * sizeof(node_t *));
    node->n = 0;
    return node;
}

void print_rec(ROOT, char* end, char* start) 
{
    char *pr = (end-3<start)?"...":end-3;
    if (p_root != NULL) {
        int i;
        printf("%s ", pr);
        for (i = 0; i < p_root->n; i++) {
            printf("%d-%s ",  p_root->p_keys[i].key, (char*)p_root->p_keys[i].p_context);
        }
        printf("\n", pr);
        for (i = 0; i < p_root->n; i++) {
            if (!p_root->leaf) {
                print_rec(p_root->pp_child[i], pr, start);
            }
        }
        if (!p_root->leaf) {
            print_rec(p_root->pp_child[i], pr, start);
        }
    }
}

void print(TREE)
{
    p("%p %p\n", p_tree, p_tree->data);
    static char prefix[] = "-----------------------------------------------------------------------";
    if(NULL != p_tree && NULL !=p_tree->data)
    {
        p("print %p->%p\n", p_tree, p_tree->data);
	    print_rec(p_tree->data, &prefix[sizeof(prefix)-2], &prefix[0]);
    }
}

#endif
