#ifndef interface_c
#define interface_c
#include <stdlib.h>
#include <sys/types.h>
#include <stdbool.h>

#define TREE treeb_t *p_tree
#define PTREE treeb_t **pp_tree
#define ROOT node_t *p_root
#define PROOT node_t **pp_root
#define SafeDelete(x)		{ if (NULL != (x)) { free(x);     (x) = NULL; } }

//#define DEBUG

#ifdef DEBUG
    #define p(...) printf("line %d ", __LINE__); printf(__VA_ARGS__);
#else
    #define p(...) /*__LINE__ __VA_ARGS__*/
#endif

#define ArraySize(x) (sizeof(x) / (sizeof((x)[0])))
#define Max(x, y) ((x)>(y)) ? (x) : (y)
#define Min(x, y) ((x)>(y)) ? (y) : (x)

typedef struct _void_ref
{
	u_int32_t  key;
	void* p_context;
} void_ref_t;

typedef struct _node
{
	void_ref_t *key;
    struct _node **child; // An array of child pointers
    u_int32_t cnt_key;     // Current number of keys
    bool leaf; // Is true when node is leaf. Otherwise false
    u_int32_t cnt_child;     // Current number of keys

} node_t;

typedef struct _treeb
{
    node_t *data;
    u_int32_t min_keys; //Minimum degree
    u_int32_t max_keys; 
    u_int32_t max_children;
} treeb_t;


bool insert(TREE, void_ref_t ref);
void delete(TREE, const u_int32_t key);
void* lookup(TREE, const u_int32_t key);

#endif
