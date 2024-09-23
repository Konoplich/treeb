#include <stdlib.h>
#include <stdio.h>
#include <libtreeb.h>
#include <stdbool.h>

void ins(PTREE, int key, char *val)
{
    void_ref_t v1 = {.key=key, .p_context=val};
    insert(*pp_tree, v1);
}
/*
struct t
{
    char *c;
    int *i;

};

struct tt
{
    struct t* s;
};

void f(struct tt** ptt)
{
    free((*ptt)->s->c);
    free((*ptt)->s->i);
    free((*ptt)->s);
    free(*ptt);
}
void test()
{
    struct tt* p = malloc(sizeof(struct tt));
    p->s = malloc(sizeof(struct t));;
    p->s->c = malloc(sizeof(char)*10);
    p->s->i = malloc(sizeof(int)*10);
    f(&p);
}
*/
int main(int argc, char** argv)
{
    //test();
    //exit(0);
    TREE;
    bool res = new(&p_tree, 2);
    printf("p_tree-%p\n", p_tree);
    if(false == res)
    {
        printf("error 1\n");
	    exit(1);
    }
    void_ref_t arr[] = 
    {
	{10, "e"}
	,{20, "e"}
	,{5, "e"}
	,{6, "e"}
	,{12, "e"}

/*
	{14, "e"}
	,{6, "e"}
	,{5, "e"}
	,{2, "e"}
	,{1, "e"}
	,{4, "e"}
    ,{9, "98"}
    ,{7, "98"}
    ,{8, "98"}
    ,{10, "98"}
    ,{11, "98"}
    ,{44, "98"}
    ,{22, "98"}
    ,{72, "98"}
    ,{63, "98"}
    ,{98, "98"}
*/    
    };
    u_int32_t max = sizeof(arr)/sizeof(void_ref_t);
    //max = 1;
    for(int i=0; i < max;i++)    {
	printf("ins %d %s\n", arr[i].key, arr[i].p_context);
	ins(&p_tree, arr[i].key, arr[i].p_context);
	print(p_tree);
	printf("\n");
    }


    print(p_tree);

    char *v;
    char *msg[] = {"found %d %s\n", "not found %d\n"};

    v = lookup(p_tree, 4);
    printf(msg[!((size_t)v | (size_t)0)], 4, v);

    v = lookup(p_tree, 5);
    printf(msg[!((size_t)v | (size_t)0)], 5, v);

    v = lookup(p_tree, 99);
    printf(msg[!((size_t)v | (size_t)0)], 99, v);


    void_ref_t arr_del[] = 
    {
	{6, "e"}

    ,{20, "98"}
/*
    ,{8, "98"}
    ,{44, "98"}
    ,{22, "98"}
    ,{98, "98"}
*/    
    };
    for(int i=0; i < sizeof(arr_del)/sizeof(void_ref_t);i++)    {
	printf("del %d %s\n", arr_del[i].key, arr_del[i].p_context);
	delete(p_tree, arr_del[i].key);
	print(p_tree);
	printf("\n");
    }
    print(p_tree);

    destructor(&p_tree);
    printf("ok\n");
    return 0;
}
