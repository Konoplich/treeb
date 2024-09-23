#ifndef delete_c
#define delete_c
#include <interface.c>
#include <stdio.h>

void copy_successor(PROOT, int pos) {
  node_t *dummy = (*pp_root)->pp_child[pos];

  for (; dummy->pp_child[0] != NULL;)
    dummy = dummy->pp_child[0];
  (*pp_root)->p_keys[pos] = dummy->p_keys[1];
}


void remove_val(PROOT, u_int32_t pos) {
  u_int32_t i = pos + 1;
    p("check i %d n %d\n", i, (*pp_root)->n) ;
  while (i <= (*pp_root)->n) {
    p("remove %d\n", (*pp_root)->p_keys[i - 1].key) ;
    (*pp_root)->p_keys[i - 1] = (*pp_root)->p_keys[i];
    (*pp_root)->pp_child[i - 1] = (*pp_root)->pp_child[i];
    i++;
  }
  (*pp_root)->n--;
}

void left_shift(PROOT, int pos) {
  int j = 1;
  node_t *x = (*pp_root)->pp_child[pos - 1];

  x->n++;
  x->p_keys[x->n] = (*pp_root)->p_keys[pos];
  x->pp_child[x->n] = (*pp_root)->pp_child[pos]->pp_child[0];

  x = (*pp_root)->pp_child[pos];
  (*pp_root)->p_keys[pos] = x->p_keys[1];
  x->pp_child[0] = x->pp_child[1];
  x->n--;

  while (j <= x->n) {
    x->p_keys[j] = x->p_keys[j + 1];
    x->pp_child[j] = x->pp_child[j + 1];
    j++;
  }
  return;
}

void merge_nodes(PROOT, int pos) {
  int j = 1;
  node_t *x1 = (*pp_root)->pp_child[pos], *x2 = (*pp_root)->pp_child[pos - 1];

  x2->n++;
  x2->p_keys[x2->n] = (*pp_root)->p_keys[pos];
  x2->pp_child[x2->n] = (*pp_root)->pp_child[0];

  while (j <= x1->n) {
    x2->n++;
    x2->p_keys[x2->n] = x1->p_keys[j];
    x2->pp_child[x2->n] = x1->pp_child[j];
    j++;
  }

  j = pos;
  while (j < (*pp_root)->n) {
    (*pp_root)->p_keys[j] = (*pp_root)->p_keys[j + 1];
    (*pp_root)->pp_child[j] = (*pp_root)->pp_child[j + 1];
    j++;
  }
  (*pp_root)->n--;
  free(x1);
}
void right_shift(PROOT, u_int32_t pos) {
  node_t *x = (*pp_root)->pp_child[pos];
  u_int32_t j = x->n;

  while (j > 0) {
    x->p_keys[j + 1] = x->p_keys[j];
    x->pp_child[j + 1] = x->pp_child[j];
  }
  x->p_keys[1] = (*pp_root)->p_keys[pos];
  x->pp_child[1] = x->pp_child[0];
  x->n++;

  x = (*pp_root)->pp_child[pos - 1];
  (*pp_root)->p_keys[pos] = x->p_keys[x->n];
  (*pp_root)->pp_child[pos] = x->pp_child[x->n];
  x->n--;
}

void adjust_node(PROOT, int pos) 
{
  if (!pos) {
    if ((*pp_root)->pp_child[1]->n > 0) {
      left_shift(pp_root, 1);
    } else {
      merge_nodes(pp_root, 1);
    }
  } else {
    if ((*pp_root)->n != pos) {
      if ((*pp_root)->pp_child[pos - 1]->n > 0) {
        right_shift(pp_root, pos);
      } else {
        if ((*pp_root)->pp_child[pos + 1]->n > 0) {
          left_shift(pp_root, pos + 1);
        } else {
          merge_nodes(pp_root, pos);
        }
      }
    } else {
      if ((*pp_root)->pp_child[pos - 1]->n > 0)
        right_shift(pp_root, pos);
      else
        merge_nodes(pp_root, pos);
    }
  }
}

int del_val_from_node(PROOT, u_int32_t key) {
  u_int32_t pos, flag = 0;
  p("1 %p\n",(*pp_root));
  if ((*pp_root)) {
    p("2 n:%d select key %d target key: %d pos: %d\n",(*pp_root)->n, (*pp_root)->p_keys[1].key, key, 1);
    if (key < (*pp_root)->p_keys[1].key) {
        p("check first key\n");
        pos = 0;
        flag = 0;
        p("pos %d\n", pos)
    } else {
        //здесь pos не инециализироан
        //p("3 scan node n: %d pos: %d %u\n", (*pp_root)->n, pos, pos);
        p("3 scan node n: %d\n", (*pp_root)->n);
      for (pos = (*pp_root)->n; (key < (*pp_root)->p_keys[pos].key && pos > 1); pos--)
      {
        p("33 key[%d]: %d\n", pos, (*pp_root)->p_keys[pos].key) ;
      }
      
        p("4 %d\n", (*pp_root)->p_keys[pos].key);
        p("44 pos %d\n", pos)
      if (key == (*pp_root)->p_keys[pos].key) {
        p("45 found pos: %d\n", pos);
        flag = 1;
      } else {
        pos--;//ищем в левой ветке
        flag = 0;
      }
    }
    p("check flag\n");
    if (flag) {
        p("found key\n");
        p("check prev %p\n", (*pp_root)->pp_child[pos-1]);
      if ((*pp_root)->pp_child[pos - 1]) {
        p("copy successor\n");
        //в приемник
        copy_successor(pp_root, pos);
        flag = del_val_from_node(&(*pp_root)->pp_child[pos], (*pp_root)->p_keys[pos].key);
        if (flag == 0) {
          printf("Given data is not present in B-Tree\n");
        }
      } else {
        p("6 remove %p pos %d key %d\n", pp_root, pos, (*pp_root)->p_keys[pos].key);
        remove_val(pp_root, pos);
      }
    } else {
        p("not found, search in child\n");
        p("7 del val from node[%d](%p)\n",pos, (*pp_root)->pp_child[pos]);
        flag = del_val_from_node(&(*pp_root)->pp_child[pos], key);
    }
    p("check child %d (%p)\n", pos, (*pp_root)->pp_child[pos]);
    if ((*pp_root)->pp_child[pos]) {
        p("8\n" );
      if ((*pp_root)->pp_child[pos]->n <= 0)
      {
            p("adjust\n");
            adjust_node(pp_root, pos);
      }
    }
  }
  return flag;
}

// Delete operaiton
void delete_rec (PROOT, const u_int32_t key) 
{
  node_t *tmp;
  if (!del_val_from_node(pp_root, key)) {
    printf("Not present\n");
    return;
  } else {
    if ((*pp_root)->n == 0) {
      tmp = (*pp_root);
      (*pp_root) = (*pp_root)->pp_child[0];
      free(tmp);
    }
  }
}
 
void delete(TREE, const u_int32_t key)
{
    if(NULL != p_tree && NULL != p_tree->data)
        delete_rec(&p_tree->data, key);
}
#endif
