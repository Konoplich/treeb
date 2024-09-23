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

node_t* new_node(u_int32_t t1, bool leaf1)
{
    p("new node %d\n", t1);
    p("new leaf %d\n", leaf1);
    p("enter\n");
    node_t* p_root = (node_t*)malloc(sizeof(node_t));
    memset(p_root, 0, sizeof(node_t));
    p("malloc %p\n",p_root);
    p("%d=%d\n", p_root->t, t1);
    // Copy the given minimum degree and leaf property
    p_root->t = t1;
    p("@1");
    p_root->leaf = leaf1;

    p("2");
    // Allocate memory for maximum number of possible keys
    // and child pointers
    //p_root->p_keys = malloc(sizeof(void_ref_t) 2*p_root->t-1);
    //p_root->p_keys = malloc(sizeof(void_ref_t) 2*p_root->t-1);
    p_root->p_keys = malloc(sizeof(void_ref_t) *2*p_root->t-sizeof(void_ref_t));
    memset(p_root->p_keys, 0, sizeof(void_ref_t) *2*p_root->t-sizeof(void_ref_t));

    p("3");
    p_root->pp_child = malloc(sizeof(node_t*) *2*p_root->t);
    memset(p_root->pp_child, 0, sizeof(node_t*) *2*p_root->t);
    // Initialize the number of keys as 0
    p_root->n = 0;
    return p_root;
}


void print_rec(ROOT, char* end, char* start)
{
    p("");
    p("%p n: %d\n", p_root, p_root->n);
    char *pr = (end-3<start)?"...":end-3;
    if(NULL != p_root->p_keys)
        for(u_int32_t i=0; i<p_root->n;i++)
            printf("%s %p [%d-%s]\n", pr,  p_root, p_root->p_keys[i].key, (char*)p_root->p_keys[i].p_context);
    if(NULL != p_root->pp_child)
        for(u_int32_t i=0; i<p_root->n && NULL != p_root->pp_child[i]; i++)
            print_rec(p_root->pp_child[i], pr, start);
}

void print(TREE)
{
    static char prefix[] = "-----------------------------------------------------------------------";
    if(NULL != p_tree && NULL !=p_tree->data)
    {
        p("print %p->%p\n", p_tree, p_tree->data);
	    print_rec(p_tree->data, &prefix[sizeof(prefix)-2], &prefix[0]);
    }
}

#endif
