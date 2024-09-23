#ifndef knowhow_c
#define knowhow_c
#include <interface.c>
#include <public.c>
#include <protected.c>
#include <private.c>
#include <stdlib.h>
#include <knowhow.h>
// A function to merge p_child[idx] with p_child[idx+1]
// p_child[idx+1] is freed after merging
void merge(TREE, ROOT, u_int32_t idx)
{
    node_t *child = p_root->pp_child[idx];
    node_t *sibling = p_root->pp_child[idx+1];

    // Pulling a key from the current node and inserting it into (t-1)th
    // position of p_child[idx]
    child->p_keys[p_tree->t-1] = p_root->p_keys[idx];

    // Copying the keys from p_child[idx+1] to p_child[idx] at the end
    for (int i=0; i<sibling->n; ++i)
        child->p_keys[i+p_tree->t] = sibling->p_keys[i];

    // Copying the child pointers from p_child[idx+1] to p_child[idx]
    if (!child->leaf)
    {
        for(int i=0; i<=sibling->n; ++i)
            child->pp_child[i+p_tree->t] = sibling->pp_child[i];
    }

    // Moving all keys after idx in the current node one step before -
    // to fill the gap created by moving keys[idx] to p_child[idx]
    for (int i=idx+1; i<p_root->n; ++i)
        p_root->p_keys[i-1] = p_root->p_keys[i];

    // Moving the child pointers after (idx+1) in the current node one
    // step before
    for (int i=idx+2; i<=p_root->n; ++i)
        p_root->pp_child[i-1] = p_root->pp_child[i];

    // Updating the key count of child and the current node
    child->n += sibling->n+1;
    p_root->n--;

    // Freeing the memory occupied by sibling
    free(sibling);
    return;
}
// A function to get predecessor of keys[idx]
void_ref_t get_pred(ROOT, u_int32_t idx)
{
    // Keep moving to the right most node until we reach a leaf
    node_t *cur=p_root->pp_child[idx];
    while (!cur->leaf)
        cur = cur->pp_child[cur->n];

    // Return the last key of the leaf
    return cur->p_keys[cur->n-1];
}


// A function to borrow a key from p_child[idx-1] and insert it
// into p_child[idx]
void borrow_from_prev(ROOT, u_int32_t idx)
{

    node_t *child=p_root->pp_child[idx];
    node_t *sibling=p_root->pp_child[idx-1];

    // The last key from p_child[idx-1] goes up to the parent and key[idx-1]
    // from parent is inserted as the first key in p_child[idx]. Thus, the  loses
    // sibling one key and child gains one key

    // Moving all key in p_child[idx] one step ahead
    for (int i=child->n-1; i>=0; --i)
        child->p_keys[i+1] = child->p_keys[i];

    // If p_child[idx] is not a leaf, move all its child pointers one step ahead
    if (!child->leaf)
    {
        for(int i=child->n; i>=0; --i)
            child->pp_child[i+1] = child->pp_child[i];
    }

    // Setting child's first key equal to keys[idx-1] from the current node
    child->p_keys[0] = p_root->p_keys[idx-1];

    // Moving sibling's last child as p_child[idx]'s first child
    if(!child->leaf)
        child->pp_child[0] = sibling->pp_child[sibling->n];

    // Moving the key from the sibling to the parent
    // This reduces the number of keys in the sibling
    p_root->p_keys[idx-1] = sibling->p_keys[sibling->n-1];

    child->n += 1;
    sibling->n -= 1;

    return;
}

// A utility function to split the child y of this node
// Note that y must be full when this function is called
void split_child(PROOT, u_int32_t i, node_t *y)
{
    p("split child\n");
    p("i %d, y: %p n:%d\n", i, y,(*pp_root)->n);
    // Create a new node which is going to store (t-1) keys
    // of y
    //node_t *z = new_node();
    node_t *z =  new_node(y->t, y->leaf);
    p("");
    z->n = (*pp_root)->t - 1;

    // Copy the last (t-1) keys of y to z
    for (int j = 0; j < (*pp_root)->t-1; j++)
        z->p_keys[j] = y->p_keys[j+(*pp_root)->t];
    
    p("");
    // Copy the last t children of y to z
    if (y->leaf == false)
    {
        for (int j = 0; j < (*pp_root)->t; j++)
            z->pp_child[j] = y->pp_child[j+(*pp_root)->t];
    }

    p("a");
    // Reduce the number of keys in y
    y->n = (*pp_root)->t - 1;

    // Since this node is going to have a new child,
    // create space of new child
    for (int j = (*pp_root)->n; j >= i+1; j--)
        (*pp_root)->pp_child[j+1] = (*pp_root)->pp_child[j];

    p("1");
    // Link the new child to this node
    (*pp_root)->pp_child[i+1] = z;

    p("2");
    // A key of y will move to this node. Find location of
    // new key and move all greater keys one space ahead
    for (int j = (*pp_root)->n-1; j >= i; j--)
        (*pp_root)->p_keys[j+1] = (*pp_root)->p_keys[j];

    // Copy the middle key of y to this node
    (*pp_root)->p_keys[i] = y->p_keys[(*pp_root)->t-1];

    p("3");
    // Increment count of keys in this node
    (*pp_root)->n = (*pp_root)->n + 1;
}
// A function to borrow a key from the p_child[idx+1] and place
// it in p_child[idx]
void borrow_from_next(ROOT, u_int32_t idx)
{

    node_t *child=p_root->pp_child[idx];
    node_t *sibling=p_root->pp_child[idx+1];

    // keys[idx] is inserted as the last key in p_child[idx]
    child->p_keys[(child->n)] = p_root->p_keys[idx];

    // Sibling's first child is inserted as the last child
    // into p_child[idx]
    if (!(child->leaf))
        child->pp_child[(child->n)+1] = sibling->pp_child[0];

    //The first key from sibling is inserted into keys[idx]
    p_root->p_keys[idx] = sibling->p_keys[0];

    // Moving all keys in sibling one step behind
    for (int i=1; i<sibling->n; ++i)
        sibling->p_keys[i-1] = sibling->p_keys[i];

    // Moving the child pointers one step behind
    if (!sibling->leaf)
    {
        for(int i=1; i<=sibling->n; ++i)
            sibling->pp_child[i-1] = sibling->pp_child[i];
    }

    // Increasing and decreasing the key count of p_child[idx] and p_child[idx+1]
    // respectively
    child->n += 1;
    sibling->n -= 1;

    return;
}

void_ref_t get_succ(ROOT, u_int32_t idx)
{

    // Keep moving the left most node starting from p_child[idx+1] until we reach a leaf
    node_t *cur = p_root->pp_child[idx+1];
    while (!cur->leaf)
        cur = cur->pp_child[0];

    // Return the first key of the leaf
    return cur->p_keys[0];
}

// A function to fill child p_child[idx] which has less than t-1 keys
void fill(TREE, ROOT, u_int32_t idx)
{

    // If the previous child(p_child[idx-1]) has more than t-1 keys, borrow a key
    // from that child
    if (idx!=0 && p_root->pp_child[idx-1]->n>=p_tree->t)
        borrow_from_prev(p_root, idx);

    // If the next child(p_child[idx+1]) has more than t-1 keys, borrow a key
    // from that child
    else if (idx!=p_root->n && p_root->pp_child[idx+1]->n>=p_tree->t)
        borrow_from_next(p_root, idx);

    // Merge p_child[idx] with its sibling
    // If p_child[idx] is the last child, merge it with its previous sibling
    // Otherwise merge it with its next sibling
    else
    {
        if (idx != p_root->n)
            merge(p_tree, p_root, idx);
        else
            merge(p_tree, p_root, idx-1);
    }
    return;
}

// A function to remove the idx-th key from this node - which is a leaf node
void remove_from_leaf (ROOT, u_int32_t idx)
{

    // Move all the keys after the idx-th pos one place backward
    for (int i=idx+1; i<p_root->n; ++i)
        p_root->p_keys[i-1] = p_root->p_keys[i];

    // Reduce the count of keys
    p_root->n--;

    return;
}

// A utility function that returns the index of the first key that is
// greater than or equal to k
u_int32_t find_key(ROOT, u_int32_t k)
{
    int idx=0;
    while (idx<p_root->n && p_root->p_keys[idx].key < k)
        ++idx;
    return idx;
}

// A function to remove the key k from the sub-tree p_rooted with this node
void remove_rec(TREE, ROOT, u_int32_t k)
{
    u_int32_t idx = find_key(p_root, k);

    // The key to be removed is present in this node
    if (idx < p_root->n && p_root->p_keys[idx].key == k)
    {

        // If the node is a leaf node - removeFromLeaf is called
        // Otherwise, removeFromNonLeaf function is called
        if (p_root->leaf)
            remove_from_leaf(p_root, idx);
        else
            remove_from_non_leaf(p_tree, p_root, idx);
    }
    else
    {

        // If this node is a leaf node, then the key is not present in tree
        if (p_root->leaf)
        {
            printf("The key %d is does not exist in the tree\n", k);
            return;
        }

        // The key to be removed is present in the sub-tree p_rooted with this node
        // The flag indicates whether the key is present in the sub-tree p_rooted
        // with the last child of this node
        bool flag = ( (idx==p_root->n)? true : false );

        // If the child where the key is supposed to exist has less that t keys,
        // we fill that child
        if (p_root->pp_child[idx]->n < p_tree->t)
            fill(p_tree, p_root, idx);

        // If the last child has been merged, it must have merged with the previous
        // child and so we recurse on the (idx-1)th child. Else, we recurse on the
        // (idx)th child which now has atleast t keys
        if (flag && idx > p_root->n)
            remove_rec(p_tree, p_root->pp_child[idx-1], k);
        else
            remove_rec(p_tree, p_root->pp_child[idx], k);
    }
    return;
}
// A function to remove the idx-th key from this node - which is a non-leaf node
void remove_from_non_leaf(TREE, ROOT, u_int32_t idx)
{

    void_ref_t k = p_root->p_keys[idx];

    // If the child that precedes k (p_child[idx]) has atleast t keys,
    // find the predecessor 'pred' of k in the subtree p_rooted at
    // p_child[idx]. Replace k by pred. Recursively delete pred
    // in p_child[idx]
    if (p_root->pp_child[idx]->n >= p_tree->t)
    {
        void_ref_t pred = get_pred(p_root, idx);
        p_root->p_keys[idx] = pred;
        remove_rec(p_tree, p_root->pp_child[idx], pred.key);
    }

    // If the child p_child[idx] has less that t keys, examine p_child[idx+1].
    // If p_child[idx+1] has atleast t keys, find the successor 'succ' of k in
    // the subtree p_rooted at p_child[idx+1]
    // Replace k by succ
    // Recursively delete succ in p_child[idx+1]
    else if  (p_root->pp_child[idx+1]->n >= p_tree->t)
    {
        void_ref_t succ = get_succ(p_root, idx);
        p_root->p_keys[idx] = succ;
        remove_rec(p_tree, p_root->pp_child[idx+1], succ.key);
    }

    // If both p_child[idx] and p_child[idx+1] has less that t keys,merge k and all of p_child[idx+1]
    // into p_child[idx]
    // Now p_child[idx] contains 2t-1 keys
    // Free p_child[idx+1] and recursively delete k from p_child[idx]
    else
    {
        merge(p_tree, p_root, idx);
        remove_rec(p_tree, p_root->pp_child[idx], k.key);
    }
    return;
}


void delete(TREE, u_int32_t key)
{
    if(NULL == p_tree || NULL == p_tree->data)
        return;
    remove_rec(p_tree, p_tree->data, key);
}

// A utility function to insert a new key in this node
// The assumption is, the node must be non-full when this
// function is called
void insert_non_full(TREE, PROOT, void_ref_t k)
{
    p("");
    // Initialize index as index of rightmost element
    int i = (*pp_root)->n-1;

    // If this is a leaf node
    if ((*pp_root)->leaf == true)
    {
        p("");
        // The following loop does two things
        // a) Finds the location of new key to be inserted
        // b) Moves all greater keys to one place ahead
        while (i >= 0 && (*pp_root)->p_keys[i].key > k.key)
        {
            (*pp_root)->p_keys[i+1] = (*pp_root)->p_keys[i];
            i--;
        }
        p("");
        // Insert the new key at found location
        (*pp_root)->p_keys[i+1] = k;
        (*pp_root)->n = (*pp_root)->n+1;
    }
    else // If this node is not leaf
    {
        p("");
        // Find the child which is going to have the new key
        while (i >= 0 && (*pp_root)->p_keys[i].key > k.key)
            i--;

        // See if the found child is full
        if ((*pp_root)->pp_child[i+1]->n == 2*p_tree->t-1)
        {
            p("");
            // If the child is full, then split it
            split_child(pp_root, i+1, (*pp_root)->pp_child[i+1]);
            p("");
            // After split, the middle key of p_child[i] goes up and
            // p_child[i] is splitted into two.  See which of the two
            // is going to have the new key
            if ((*pp_root)->p_keys[i+1].key < k.key)
                i++;
        }
        p("");
        insert_non_full(p_tree, &(*pp_root)->pp_child[i+1], k);
    }
}

// The main function that inserts a new key in this B-Tree
bool insert_rec(TREE, PROOT, void_ref_t k)
{
    p("insert rec pp_root: %p\n",pp_root);
    // If tree is empty
    if (*pp_root == NULL)
    {

        p("insert empty t:%d\n", p_tree->t);
        // Allocate memory for p_root
        //p_root = new_node();
        *pp_root = new_node(p_tree->t, true);

        p("");
        (*pp_root)->p_keys[0] = k;  // Insert key
        (*pp_root)->n = 1;  // Update number of keys in p_root
        p("data: %p root %p n:%d t:%d\n", p_tree->data, *pp_root, (*pp_root)->n, (*pp_root)->t);
    }
    else // If tree is not empty
    {
        p("insert non empty\n");
        // If p_root is full, then tree grows in height
        //if (p_root->n == 2*p_tree->t-1)
        if ((*pp_root)->n == 2*p_tree->t-1)
        {
            p("eq\n");
            // Allocate memory for new p_root
            //node_t *s = new_node();
            node_t *s = new_node(p_tree->t, false);

            p("n: %d\n", s->n);
            // Make old p_root as child of new p_root
            s->pp_child[0] = p_tree->data;

            p("22 n:%d\n", s->n);
            // Split the old p_root and move 1 key to the new p_root
            split_child(&s, 0, p_tree->data);

            p("23");
            // New p_root has two children now.  Decide which of the
            // two children is going to have new key
            int i = 0;
            if (s->p_keys[0].key < k.key)
                i++;
            
            p("");
            insert_non_full(p_tree, &s->pp_child[i], k);

            // Change p_root
            *pp_root = s;
        }
        else  // If p_root is not full, call insertNonFull for p_root
            insert_non_full(p_tree, pp_root, k);
        
    }
    
    p("");
    return true;
}

bool insert(TREE, void_ref_t key)
{
    if(NULL == p_tree)
    {
        return false;
    }
    return insert_rec(p_tree, &p_tree->data, key);
}
/*

// Function to traverse all nodes in a subtree p_rooted with this node
void traverse_rec(char *prefix, char* begin, ROOT)
{
    char *pr = ((prefix-3) <= begin) ? begin : prefix -3);

    // There are n keys and n+1 children, traverse through n keys
    // and first n children
    int i;
    for (i = 0; i < p_root->n; i++)
    {
        // If this is not leaf, then before printing key[i],
        // traverse the subtree p_rooted with child p_child[i].
        if (p_root->leaf == false)
            traverse_rec(pr, begin, p_root->pp_child[i]);
        printf(" %d-%s", p_root->p_keys[i].key, p_root->p_keys[i].p_context);
    }

    // Print the subtree p_rooted with last child
    if (p_root->leaf == false)
        traverse_rec(pr, begin, p_root->pp_child[i]);
}

void traverse(TREE)
{
    static char *p ="--------------------------------------------------------------------------------------------";
    if(NULL == p_tree || NULL == p_tree->data)
    {
        printf("empty\n");
        return;
    }
    traverse_rec(p+sizeof(*p)-(3+1), p, p_tree->data); //3 символа + ноль в конце
}

// Function to search key k in subtree p_rooted with this node
node_t *lookup_rec(ROOT, void_ref_t k)
{
    // Find the first key greater than or equal to k
    int i = 0;
    while (i < p_root->n && k.key > p_root->p_keys[i].key)
        i++;

    // If the found key is equal to k, return this node
    if (p_root->p_keys[i].key == k.key)
        return p_root;

    // If key is not found here and this is a leaf node
    if (p_root->leaf == true)
        return NULL;

    // Go to the appropriate child
    return lookup_rec(p_root->pp_child[i], k);

}

node_t* lookup(TREE, void_ref_t key)
{
    if(NULL == p_tree || NULL == p_tree->data)
        return NULL;
    return lookup_rec(p_tree->data, key);
}

void remove_rec(ROOT, void_ref_t k)
{
    if (!p_root)
    {
        printf("The tree is empty\n");
        return;
    }

    // Call the remove function for p_root
    remove_rec(p_root, k);

    // If the p_root node has 0 keys, make its first child as the new p_root
    //  if it has a child, otherwise set p_root as NULL
    if (p_root->n==0)
    {
        node_t *tmp = p_root;
        if (p_root->leaf)
            p_root = NULL;
        else
            p_root = p_root->pp_child[0];

        // Free the old p_root
        free(tmp);
    }
    return;
}

bool remove(TREE, void_ref_t key)
{
    if(NULL == p_tree)
        return false;
    return remove_rec(p_tree->data, key);
}

*/
#endif
