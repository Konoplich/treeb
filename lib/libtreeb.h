#ifndef lib
#define lib

#include <stdlib.h>
#include <sys/types.h>
#include <stdbool.h>

#define TREE treeb_t *p_tree
#define PTREE treeb_t **pp_tree
#define ROOT node_t *p_root
#define PROOT node_t **pp_root
#define SafeDelete(x)		{ if (NULL != (x)) { free(x);     (x) = NULL; } }


typedef struct _void_ref
{
	u_int32_t  key;
	void* p_context;
} void_ref_t;

typedef struct _node
{
    void_ref_t *p_keys;
    u_int32_t t;      // Minimum degree (defines the range for number of keys)
    struct _node **pp_child; // An array of child pointers
    u_int32_t n;     // Current number of keys
    bool leaf; // Is true when node is leaf. Otherwise false

} node_t;

typedef struct _treeb
{
    node_t *data;
    u_int32_t t; //Minimum degree
} treeb_t;

bool insert(TREE, void_ref_t ref);
void delete(TREE, const u_int32_t key);
void* lookup(TREE, const u_int32_t key);

bool new(PTREE, u_int32_t min_degree);
void destructor(PTREE);
void print(TREE);
//void leaf_depth(TREE);
//void traverse(TREE);
//u_int32_t key_count(TREE);
//void sanity_check(TREE);
#endif
