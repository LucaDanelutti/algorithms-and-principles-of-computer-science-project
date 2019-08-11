#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define REL_HASH_SIZE 20
#define REL_ID_SIZE 35 //Poni uguale a ENT_ID_SIZE
#define ENT_ID_SIZE 35
#define LINE_SIZE 100

const int RED = 0;
const int BLACK = 1;

typedef struct rb_node *rbNode;
typedef struct rb_node *rbTree;

typedef struct rb_node_REPORT *rbNode_REPORT;
typedef struct rb_node_REPORT *rbTree_REPORT;

typedef struct rb_node_RELSHIP *rbNode_RELSHIP;
typedef struct rb_node_RELSHIP *rbTree_RELSHIP;

struct hashtable *rel_hashtable = NULL;

struct hashtable_node
{
    char key[REL_ID_SIZE];
    rbTree_REPORT *report_rb_pointer;
    rbTree_RELSHIP *relship_rb_pointer;
    int max;
    struct hashtable_node *next;
};

struct hashtable
{
    int size;
    struct hashtable_node **list;
};

struct hashtable *hashtable_createTable(int size)
{
    struct hashtable *t = (struct hashtable *)malloc(sizeof(struct hashtable));
    t->size = size;
    t->list = (struct hashtable_node **)malloc(sizeof(struct hashtable_node *) * size);
    int i;
    for (i = 0; i < size; i++)
        t->list[i] = NULL;
    return t;
}

int hashtable_hashFunction(struct hashtable *t, char *key)
{
    int pos = 0;
    for (int i = 0; i < REL_ID_SIZE && key[i] != '\0'; i++)
    {
        pos = pos + key[i];
    }
    return pos % REL_HASH_SIZE;
}

void hashtable_insert(struct hashtable *t, char *key, rbTree_REPORT *report_rb_pointer, rbTree_RELSHIP *relship_rb_pointer, int max)
{
    int pos = hashtable_hashFunction(t, key);
    struct hashtable_node *list = t->list[pos];
    struct hashtable_node *newNode = (struct hashtable_node *)malloc(sizeof(struct hashtable_node));
    struct hashtable_node *temp = list;
    while (temp)
    {
        if (temp->key == key)
        {
            temp->report_rb_pointer = report_rb_pointer;
            return;
        }
        temp = temp->next;
    }
    strcpy(newNode->key, key);
    newNode->report_rb_pointer = report_rb_pointer;
    newNode->relship_rb_pointer = relship_rb_pointer;
    newNode->max = max;
    newNode->next = list;
    t->list[pos] = newNode;
}

struct hashtable_node *hashtable_search(struct hashtable *t, char *key)
{
    int pos = hashtable_hashFunction(t, key);
    struct hashtable_node *list = t->list[pos];
    struct hashtable_node *temp = list;
    while (temp)
    {
        if (!strcmp(temp->key, key))
        {
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

//##############
//RB-TREE STRING
//##############

struct rb_node
{
    char key[ENT_ID_SIZE];
    int color;
    rbNode parent, left, right;
};

static struct rb_node sentinel = {"0", 1, 0, 0, 0};
#define NIL &sentinel

rbTree ent_rb = NIL;
rbTree rel_rb = NIL;

rbNode rb_search(rbTree tree, char *k)
{
    if (tree == NIL || strcmp(k, tree->key) == 0)
    {
        return tree;
    }
    if (strcmp(k, tree->key) < 0)
    {
        return rb_search(tree->left, k);
    }
    else
    {
        return rb_search(tree->right, k);
    }
}

rbNode rb_minimum(rbTree tree)
{
    while (tree->left != NIL)
    {
        tree = tree->left;
    }
    return tree;
}

rbNode rb_maximum(rbTree tree)
{
    while (tree->right != NIL)
    {
        tree = tree->right;
    }
    return tree;
}

rbNode rb_successor(rbNode x)
{
    if (x->right != NIL)
    {
        return rb_minimum(x->right);
    }
    rbNode y = x->parent;
    while (y != NIL && x == y->right)
    {
        x = y;
        y = y->parent;
    }
    return y;
}

void rb_leftRotate(rbTree *tree, rbNode x)
{
    rbNode y = x->right;
    x->right = y->left;
    if (y->left != NIL)
    {
        y->left->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == NIL)
    {
        *tree = y;
    }
    else if (x == x->parent->left)
    {
        x->parent->left = y;
    }
    else
    {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

void rb_rightRotate(rbTree *tree, rbNode x)
{
    rbNode y = x->left;
    x->left = y->right;
    if (y->right != NIL)
    {
        y->right->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == NIL)
    {
        *tree = y;
    }
    else if (x == x->parent->left)
    {
        x->parent->left = y;
    }
    else
    {
        x->parent->right = y;
    }
    y->right = x;
    x->parent = y;
}

void rb_insertFixUp(rbTree *tree, rbNode z)
{
    while (z->parent->color == RED)
    {
        if (z->parent == z->parent->parent->left)
        {
            rbNode y = z->parent->parent->right;
            if (y->color == RED)
            { //Case1
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            }
            else
            {
                if (z == z->parent->right)
                { //Case2
                    z = z->parent;
                    rb_leftRotate(tree, z); //case2->case3
                }
                z->parent->color = BLACK; //Case3
                z->parent->parent->color = RED;
                rb_rightRotate(tree, z->parent->parent);
            }
        }
        else
        {
            rbNode y = z->parent->parent->left;
            if (y->color == RED)
            {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            }
            else
            {
                if (z == z->parent->left)
                { //Case2
                    z = z->parent;
                    rb_rightRotate(tree, z); //case2->case3
                }
                z->parent->color = BLACK; //Case3
                z->parent->parent->color = RED;
                rb_leftRotate(tree, z->parent->parent);
            }
        }
    }
    (*tree)->color = BLACK;
}

int rb_insert(rbTree *tree, char *k)
{
    rbNode y = NIL;
    rbNode x = *tree;
    rbNode z = (struct rb_node *)malloc(sizeof(struct rb_node));
    strcpy(z->key, k);
    z->parent = z->left = z->right = NIL;

    while (x != NIL)
    {
        y = x;
        if (strcmp(z->key, x->key) == 0)
            return 0;
        if (strcmp(z->key, x->key) < 0)
        {
            x = x->left;
        }
        else
        {
            x = x->right;
        }
    }
    z->parent = y;
    if (y == NIL)
    {
        *tree = z;
    }
    else
    {
        if (strcmp(z->key, y->key) < 0)
        {
            y->left = z;
        }
        else
        {
            y->right = z;
        }
    }
    z->left = z->right = NIL;
    z->color = RED;
    rb_insertFixUp(tree, z);
    return 1;
}

void rb_removeFixUp(rbTree *tree, rbNode x)
{
    while (x != *tree && x->color == BLACK)
    {
        if (x == x->parent->left)
        {
            rbNode w = x->parent->right;
            if (w->color == RED)
            { //Case1
                w->color = BLACK;
                x->parent->color = RED;
                rb_leftRotate(tree, x->parent);
                w = x->parent->right;
            }
            //Case2
            if (w->left->color == BLACK && w->right->color == BLACK)
            {
                w->color = RED;
                x = x->parent;
            }
            else if (w->right->color == BLACK)
            { //Case3->Case4
                w->left->color = BLACK;
                w->color = RED;
                rb_rightRotate(tree, w);
                w = x->parent->right;
            }
            else
            { //Case4
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->right->color = BLACK;
                rb_leftRotate(tree, x->parent);
                x = *tree;
            }
        }
        else
        {
            rbNode w = x->parent->left;
            if (w->color == RED)
            { //Case1
                w->color = BLACK;
                x->parent->color = RED;
                rb_rightRotate(tree, x->parent);
                w = x->parent->left;
            }
            //Case2
            if (w->left->color == BLACK && w->right->color == BLACK)
            {
                w->color = RED;
                x = x->parent;
            }
            else if (w->left->color == BLACK)
            { //Case3->Case4
                w->right->color = BLACK;
                w->color = RED;
                rb_leftRotate(tree, w);
                w = x->parent->left;
            }
            else
            { //Case4
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->left->color = BLACK;
                rb_rightRotate(tree, x->parent);
                x = *tree;
            }
        }
    }
    x->color = BLACK;
}

rbNode rb_remove(rbTree *tree, rbNode z)
{
    rbNode x, y;
    if (z->left == NIL || z->right == NIL)
    {
        y = z;
    }
    else
    {
        y = rb_successor(z);
    }
    if (y->left != NIL)
    {
        x = y->left;
    }
    else
    {
        x = y->right;
    }
    x->parent = y->parent;
    if (y->parent == NIL)
    {
        *tree = x;
    }
    else if (y == y->parent->left)
    {
        y->parent->left = x;
    }
    else
    {
        y->parent->right = x;
    }
    if (y != z)
    {
        strcpy(z->key, y->key);
    }
    if (y->color == BLACK)
    {
        rb_removeFixUp(tree, x);
    }
    return y;
}

void rb_freeNode(rbNode node)
{
    free(node);
}

void rb_destroy(rbTree tree)
{
    if (tree == NIL)
        return;
    rb_destroy(tree->left);
    rb_destroy(tree->right);
    free(tree);
}

void rb_inOrder(rbTree tree)
{
    if (tree == NIL)
        return;

    rb_inOrder(tree->left);
    printf("%s ", tree->key);
    rb_inOrder(tree->right);
}

//##############
//RB-TREE REPORT
//##############

struct rb_node_REPORT
{
    char key[ENT_ID_SIZE];
    int count;
    int color;
    rbNode_REPORT parent, left, right;
};

static struct rb_node_REPORT sentinel_REPORT = {"0", 0, 1, 0, 0, 0};
#define NIL_REPORT &sentinel_REPORT

rbNode_REPORT rb_search_REPORT(rbTree_REPORT tree, char *k)
{
    if (tree == NIL_REPORT || strcmp(k, tree->key) == 0)
    {
        return tree;
    }
    if (strcmp(k, tree->key) < 0)
    {
        return rb_search_REPORT(tree->left, k);
    }
    else
    {
        return rb_search_REPORT(tree->right, k);
    }
}

rbNode_REPORT rb_minimum_REPORT(rbTree_REPORT tree)
{
    while (tree->left != NIL_REPORT)
    {
        tree = tree->left;
    }
    return tree;
}

rbNode_REPORT rb_maximum_REPORT(rbTree_REPORT tree)
{
    while (tree->right != NIL_REPORT)
    {
        tree = tree->right;
    }
    return tree;
}

rbNode_REPORT rb_successor_REPORT(rbNode_REPORT x)
{
    if (x->right != NIL_REPORT)
    {
        return rb_minimum_REPORT(x->right);
    }
    rbNode_REPORT y = x->parent;
    while (y != NIL_REPORT && x == y->right)
    {
        x = y;
        y = y->parent;
    }
    return y;
}

void rb_leftRotate_REPORT(rbTree_REPORT *tree, rbNode_REPORT x)
{
    rbNode_REPORT y = x->right;
    x->right = y->left;
    if (y->left != NIL_REPORT)
    {
        y->left->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == NIL_REPORT)
    {
        *tree = y;
    }
    else if (x == x->parent->left)
    {
        x->parent->left = y;
    }
    else
    {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

void rb_rightRotate_REPORT(rbTree_REPORT *tree, rbNode_REPORT x)
{
    rbNode_REPORT y = x->left;
    x->left = y->right;
    if (y->right != NIL_REPORT)
    {
        y->right->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == NIL_REPORT)
    {
        *tree = y;
    }
    else if (x == x->parent->left)
    {
        x->parent->left = y;
    }
    else
    {
        x->parent->right = y;
    }
    y->right = x;
    x->parent = y;
}

void rb_insertFixUp_REPORT(rbTree_REPORT *tree, rbNode_REPORT z)
{
    while (z->parent->color == RED)
    {
        if (z->parent == z->parent->parent->left)
        {
            rbNode_REPORT y = z->parent->parent->right;
            if (y->color == RED)
            { //Case1
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            }
            else
            {
                if (z == z->parent->right)
                { //Case2
                    z = z->parent;
                    rb_leftRotate_REPORT(tree, z); //case2->case3
                }
                z->parent->color = BLACK; //Case3
                z->parent->parent->color = RED;
                rb_rightRotate_REPORT(tree, z->parent->parent);
            }
        }
        else
        {
            rbNode_REPORT y = z->parent->parent->left;
            if (y->color == RED)
            {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            }
            else
            {
                if (z == z->parent->left)
                { //Case2
                    z = z->parent;
                    rb_rightRotate_REPORT(tree, z); //case2->case3
                }
                z->parent->color = BLACK; //Case3
                z->parent->parent->color = RED;
                rb_leftRotate_REPORT(tree, z->parent->parent);
            }
        }
    }
    (*tree)->color = BLACK;
}

void rb_insert_REPORT(rbTree_REPORT *tree, char *k, int count)
{
    rbNode_REPORT y = NIL_REPORT;
    rbNode_REPORT x = *tree;
    rbNode_REPORT z = (struct rb_node_REPORT *)malloc(sizeof(struct rb_node_REPORT));
    strcpy(z->key, k);
    z->count = count;
    z->parent = z->left = z->right = NIL_REPORT;

    while (x != NIL_REPORT)
    {
        y = x;
        if (strcmp(z->key, x->key) < 0)
        {
            x = x->left;
        }
        else
        {
            x = x->right;
        }
    }
    z->parent = y;
    if (y == NIL_REPORT)
    {
        *tree = z;
    }
    else
    {
        if (strcmp(z->key, y->key) < 0)
        {
            y->left = z;
        }
        else
        {
            y->right = z;
        }
    }
    z->left = z->right = NIL_REPORT;
    z->color = RED;
    rb_insertFixUp_REPORT(tree, z);
}

void rb_removeFixUp_REPORT(rbTree_REPORT *tree, rbNode_REPORT x)
{
    while (x != *tree && x->color == BLACK)
    {
        if (x == x->parent->left)
        {
            rbNode_REPORT w = x->parent->right;
            if (w->color == RED)
            { //Case1
                w->color = BLACK;
                x->parent->color = RED;
                rb_leftRotate_REPORT(tree, x->parent);
                w = x->parent->right;
            }
            //Case2
            if (w->left->color == BLACK && w->right->color == BLACK)
            {
                w->color = RED;
                x = x->parent;
            }
            else if (w->right->color == BLACK)
            { //Case3->Case4
                w->left->color = BLACK;
                w->color = RED;
                rb_rightRotate_REPORT(tree, w);
                w = x->parent->right;
            }
            else
            { //Case4
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->right->color = BLACK;
                rb_leftRotate_REPORT(tree, x->parent);
                x = *tree;
            }
        }
        else
        {
            rbNode_REPORT w = x->parent->left;
            if (w->color == RED)
            { //Case1
                w->color = BLACK;
                x->parent->color = RED;
                rb_rightRotate_REPORT(tree, x->parent);
                w = x->parent->left;
            }
            //Case2
            if (w->left->color == BLACK && w->right->color == BLACK)
            {
                w->color = RED;
                x = x->parent;
            }
            else if (w->left->color == BLACK)
            { //Case3->Case4
                w->right->color = BLACK;
                w->color = RED;
                rb_leftRotate_REPORT(tree, w);
                w = x->parent->left;
            }
            else
            { //Case4
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->left->color = BLACK;
                rb_rightRotate_REPORT(tree, x->parent);
                x = *tree;
            }
        }
    }
    x->color = BLACK;
}

rbNode_REPORT rb_remove_REPORT(rbTree_REPORT *tree, rbNode_REPORT z)
{
    rbNode_REPORT x, y;
    if (z->left == NIL_REPORT || z->right == NIL_REPORT)
    {
        y = z;
    }
    else
    {
        y = rb_successor_REPORT(z);
    }
    if (y->left != NIL_REPORT)
    {
        x = y->left;
    }
    else
    {
        x = y->right;
    }
    x->parent = y->parent;
    if (y->parent == NIL_REPORT)
    {
        *tree = x;
    }
    else if (y == y->parent->left)
    {
        y->parent->left = x;
    }
    else
    {
        y->parent->right = x;
    }
    if (y != z)
    {
        strcpy(z->key, y->key);
        z->count = y->count;
    }
    if (y->color == BLACK)
    {
        rb_removeFixUp_REPORT(tree, x);
    }
    return y;
}

void rb_destroy_REPORT(rbTree_REPORT tree)
{
    if (tree == NIL_REPORT)
        return;
    rb_destroy_REPORT(tree->left);
    rb_destroy_REPORT(tree->right);
    free(tree);
}

void rb_inOrder_REPORT(rbTree_REPORT tree)
{
    if (tree == NIL_REPORT)
        return;

    rb_inOrder_REPORT(tree->left);
    printf("### %s with %d ###", tree->key, tree->count);
    rb_inOrder_REPORT(tree->right);
}

int rb_findMax_REPORT(rbTree_REPORT tree)
{
    int max;
    int left;
    int right;
    if (tree == NIL_REPORT)
        return 0;

    left = rb_findMax_REPORT(tree->left);
    right = rb_findMax_REPORT(tree->right);
    if (tree->count > left)
        max = tree->count;
    else
        max = left;
    if (max > right)
        return max;
    return right;
}

//###############
//RB-TREE RELSHIP
//###############

struct rb_node_RELSHIP
{
    char key[ENT_ID_SIZE * 2];
    int color;
    rbNode_RELSHIP parent, left, right;
};

static struct rb_node_RELSHIP sentinel_RELSHIP = {"0", 1, 0, 0, 0};
#define NIL_RELSHIP &sentinel_RELSHIP

rbNode_RELSHIP rb_search_RELSHIP(rbTree_RELSHIP tree, char *k)
{
    if (tree == NIL_RELSHIP || strcmp(k, tree->key) == 0)
    {
        return tree;
    }
    if (strcmp(k, tree->key) < 0)
    {
        return rb_search_RELSHIP(tree->left, k);
    }
    else
    {
        return rb_search_RELSHIP(tree->right, k);
    }
}

rbNode_RELSHIP rb_minimum_RELSHIP(rbTree_RELSHIP tree)
{
    while (tree->left != NIL_RELSHIP)
    {
        tree = tree->left;
    }
    return tree;
}

rbNode_RELSHIP rb_maximum_RELSHIP(rbTree_RELSHIP tree)
{
    while (tree->right != NIL_RELSHIP)
    {
        tree = tree->right;
    }
    return tree;
}

rbNode_RELSHIP rb_successor_RELSHIP(rbNode_RELSHIP x)
{
    if (x->right != NIL_RELSHIP)
    {
        return rb_minimum_RELSHIP(x->right);
    }
    rbNode_RELSHIP y = x->parent;
    while (y != NIL_RELSHIP && x == y->right)
    {
        x = y;
        y = y->parent;
    }
    return y;
}

void rb_leftRotate_RELSHIP(rbTree_RELSHIP *tree, rbNode_RELSHIP x)
{
    rbNode_RELSHIP y = x->right;
    x->right = y->left;
    if (y->left != NIL_RELSHIP)
    {
        y->left->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == NIL_RELSHIP)
    {
        *tree = y;
    }
    else if (x == x->parent->left)
    {
        x->parent->left = y;
    }
    else
    {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

void rb_rightRotate_RELSHIP(rbTree_RELSHIP *tree, rbNode_RELSHIP x)
{
    rbNode_RELSHIP y = x->left;
    x->left = y->right;
    if (y->right != NIL_RELSHIP)
    {
        y->right->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == NIL_RELSHIP)
    {
        *tree = y;
    }
    else if (x == x->parent->left)
    {
        x->parent->left = y;
    }
    else
    {
        x->parent->right = y;
    }
    y->right = x;
    x->parent = y;
}

void rb_insertFixUp_RELSHIP(rbTree_RELSHIP *tree, rbNode_RELSHIP z)
{
    while (z->parent->color == RED)
    {
        if (z->parent == z->parent->parent->left)
        {
            rbNode_RELSHIP y = z->parent->parent->right;
            if (y->color == RED)
            { //Case1
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            }
            else
            {
                if (z == z->parent->right)
                { //Case2
                    z = z->parent;
                    rb_leftRotate_RELSHIP(tree, z); //case2->case3
                }
                z->parent->color = BLACK; //Case3
                z->parent->parent->color = RED;
                rb_rightRotate_RELSHIP(tree, z->parent->parent);
            }
        }
        else
        {
            rbNode_RELSHIP y = z->parent->parent->left;
            if (y->color == RED)
            {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            }
            else
            {
                if (z == z->parent->left)
                { //Case2
                    z = z->parent;
                    rb_rightRotate_RELSHIP(tree, z); //case2->case3
                }
                z->parent->color = BLACK; //Case3
                z->parent->parent->color = RED;
                rb_leftRotate_RELSHIP(tree, z->parent->parent);
            }
        }
    }
    (*tree)->color = BLACK;
}

int rb_insert_RELSHIP(rbTree_RELSHIP *tree, char *k)
{
    rbNode_RELSHIP y = NIL_RELSHIP;
    rbNode_RELSHIP x = *tree;
    rbNode_RELSHIP z = (struct rb_node_RELSHIP *)malloc(sizeof(struct rb_node_RELSHIP));
    strcpy(z->key, k);
    z->parent = z->left = z->right = NIL_RELSHIP;

    while (x != NIL_RELSHIP)
    {
        y = x;
        if (strcmp(z->key, x->key) == 0)
            return 0;
        if (strcmp(z->key, x->key) < 0)
        {
            x = x->left;
        }
        else
        {
            x = x->right;
        }
    }
    z->parent = y;
    if (y == NIL_RELSHIP)
    {
        *tree = z;
    }
    else
    {
        if (strcmp(z->key, y->key) < 0)
        {
            y->left = z;
        }
        else
        {
            y->right = z;
        }
    }
    z->left = z->right = NIL_RELSHIP;
    z->color = RED;
    rb_insertFixUp_RELSHIP(tree, z);
    return 1;
}

void rb_removeFixUp_RELSHIP(rbTree_RELSHIP *tree, rbNode_RELSHIP x)
{
    while (x != *tree && x->color == BLACK)
    {
        if (x == x->parent->left)
        {
            rbNode_RELSHIP w = x->parent->right;
            if (w->color == RED)
            { //Case1
                w->color = BLACK;
                x->parent->color = RED;
                rb_leftRotate_RELSHIP(tree, x->parent);
                w = x->parent->right;
            }
            //Case2
            if (w->left->color == BLACK && w->right->color == BLACK)
            {
                w->color = RED;
                x = x->parent;
            }
            else if (w->right->color == BLACK)
            { //Case3->Case4
                w->left->color = BLACK;
                w->color = RED;
                rb_rightRotate_RELSHIP(tree, w);
                w = x->parent->right;
            }
            else
            { //Case4
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->right->color = BLACK;
                rb_leftRotate_RELSHIP(tree, x->parent);
                x = *tree;
            }
        }
        else
        {
            rbNode_RELSHIP w = x->parent->left;
            if (w->color == RED)
            { //Case1
                w->color = BLACK;
                x->parent->color = RED;
                rb_rightRotate_RELSHIP(tree, x->parent);
                w = x->parent->left;
            }
            //Case2
            if (w->left->color == BLACK && w->right->color == BLACK)
            {
                w->color = RED;
                x = x->parent;
            }
            else if (w->left->color == BLACK)
            { //Case3->Case4
                w->right->color = BLACK;
                w->color = RED;
                rb_leftRotate_RELSHIP(tree, w);
                w = x->parent->left;
            }
            else
            { //Case4
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->left->color = BLACK;
                rb_rightRotate_RELSHIP(tree, x->parent);
                x = *tree;
            }
        }
    }
    x->color = BLACK;
}

rbNode_RELSHIP rb_remove_RELSHIP(rbTree_RELSHIP *tree, rbNode_RELSHIP z)
{
    rbNode_RELSHIP x, y;
    if (z->left == NIL_RELSHIP || z->right == NIL_RELSHIP)
    {
        y = z;
    }
    else
    {
        y = rb_successor_RELSHIP(z);
    }
    if (y->left != NIL_RELSHIP)
    {
        x = y->left;
    }
    else
    {
        x = y->right;
    }
    x->parent = y->parent;
    if (y->parent == NIL_RELSHIP)
    {
        *tree = x;
    }
    else if (y == y->parent->left)
    {
        y->parent->left = x;
    }
    else
    {
        y->parent->right = x;
    }
    if (y != z)
    {
        strcpy(z->key, y->key);
    }
    if (y->color == BLACK)
    {
        rb_removeFixUp_RELSHIP(tree, x);
    }
    return y;
}

void rb_destroy_RELSHIP(rbTree_RELSHIP tree)
{
    if (tree == NIL_RELSHIP)
        return;
    rb_destroy_RELSHIP(tree->left);
    rb_destroy_RELSHIP(tree->right);
    free(tree);
}

void rb_inOrder_RELSHIP(rbTree_RELSHIP tree)
{
    if (tree == NIL_RELSHIP)
        return;

    rb_inOrder_RELSHIP(tree->left);
    printf("#%s# ; ", tree->key);
    rb_inOrder_RELSHIP(tree->right);
}

rbNode_RELSHIP rb_searchTo_RELSHIP(rbTree_RELSHIP tree, char *entToID)
{
    if (tree == NIL_RELSHIP)
        return tree;

    char relationship[REL_ID_SIZE * 2];
    char *substring;
    strcpy(relationship, tree->key);
    substring = strtok(relationship, ".");
    //substring = strtok(NULL, "\"");
    if (strcmp(entToID, substring) == 0)
    {
        return tree;
    }
    if (strcmp(entToID, substring) < 0)
    {
        return rb_searchTo_RELSHIP(tree->left, entToID);
    }
    else
    {
        return rb_searchTo_RELSHIP(tree->right, entToID);
    }
}

rbNode_RELSHIP rb_searchFrom_RELSHIP(rbTree_RELSHIP tree, char *entFromID)
{
    if (tree == NIL_RELSHIP)
        return tree;

    char relationship[REL_ID_SIZE * 2];
    char *substring;
    strcpy(relationship, tree->key);
    substring = strtok(relationship, ".");
    substring = strtok(NULL, ".");
    if (strcmp(entFromID, substring) == 0)
    {
        return tree;
    }
    if (strcmp(entFromID, substring) < 0)
    {
        return rb_searchFrom_RELSHIP(tree->left, entFromID);
    }
    else
    {
        return rb_searchFrom_RELSHIP(tree->right, entFromID);
    }
}

char *rb_To_RELSHIP(rbTree_RELSHIP tree, rbNode_RELSHIP node)
{
    char relationship[REL_ID_SIZE * 2];
    char *substring;
    strcpy(relationship, tree->key);
    substring = strtok(relationship, ".");
    return substring;
}

void print_max(rbTree_REPORT tree, int max)
{
    if (tree == NULL)
        return;
    print_max(tree->left, max);
    //printf("# %d #", tree->count);
    if (tree->count == max)
    {
        fputs("\"", stdout);
        fputs(tree->key, stdout);
        fputs("\" ", stdout);
    }
    print_max(tree->right, max);
}

void report_recursive(rbTree rel_rb)
{
    struct hashtable_node *node;
    if (rel_rb == NIL)
        return;
    report_recursive(rel_rb->left);
    node = hashtable_search(rel_hashtable, rel_rb->key);
    if (node != NULL && node->max > 0)
    {
        fputs("\"", stdout);
        fputs(rel_rb->key, stdout);
        fputs("\" ", stdout);
        print_max(*(node->report_rb_pointer), node->max);
        printf("%d", node->max);
        fputs("; ", stdout);
    }
    report_recursive(rel_rb->right);
    return;
}

void addent(char *entID)
{
    //printf("Adding ent %s\n", entID);
    if (rb_search(ent_rb, entID) == NIL)
        rb_insert(&ent_rb, entID);
}

void addrel(char *relID, char *entFromID, char *entToID)
{
    if (rb_search(ent_rb, entFromID) == NIL || rb_search(ent_rb, entToID) == NIL)
    {
        //printf("Relationship %s between %s and %s not added!", relID, entID1, entID2);
        return;
    }
    //printf("Adding relationship %s between %s and %s\n", relID, entFromID, entToID);
    struct hashtable_node *hash_node;
    rbTree_REPORT *buffer;
    rbTree_RELSHIP *buffer1;
    char relationship[REL_ID_SIZE * 2];
    hash_node = hashtable_search(rel_hashtable, relID);
    if (hash_node == NULL)
    {
        buffer = malloc(sizeof(void *));
        *buffer = NIL_REPORT;
        buffer1 = malloc(sizeof(void *));
        *buffer1 = NIL_RELSHIP;
        hashtable_insert(rel_hashtable, relID, buffer, buffer1, 0);
    }
    hash_node = hashtable_search(rel_hashtable, relID);
    rb_insert(&rel_rb, relID);
    strcpy(relationship, entToID);
    strcat(relationship, ".");
    strcat(relationship, entFromID);
    if (rb_insert_RELSHIP(hash_node->relship_rb_pointer, relationship) == 1)
    {
        rbNode_REPORT report_rb_node = rb_search_REPORT(*(hash_node->report_rb_pointer), entToID);
        if (report_rb_node == NIL_REPORT)
        {
            rb_insert_REPORT(hash_node->report_rb_pointer, entToID, 1);
            report_rb_node = rb_search_REPORT(*(hash_node->report_rb_pointer), entToID);
        }
        else
        {
            report_rb_node->count++;
        }
        if (report_rb_node->count > hash_node->max)
            hash_node->max = report_rb_node->count;
    }
    return;
}

void delrel(char *relID, char *entFromID, char *entToID)
{
    struct hashtable_node *hash_node;
    char relationship[REL_ID_SIZE * 2];
    hash_node = hashtable_search(rel_hashtable, relID);
    if (hash_node != NULL)
    {
        strcpy(relationship, entToID);
        strcat(relationship, ".");
        strcat(relationship, entFromID);
        rbNode_RELSHIP rb_relship_node = rb_search_RELSHIP(*(hash_node->relship_rb_pointer), relationship);
        if (rb_relship_node != NIL_RELSHIP)
        {
            rbNode_RELSHIP toFree = rb_remove_RELSHIP(hash_node->relship_rb_pointer, rb_relship_node);
            free(toFree);
            rbNode_REPORT rb_report_node = rb_search_REPORT(*(hash_node->report_rb_pointer), entToID);
            rb_report_node->count--;
            if (rb_report_node->count + 1 == hash_node->max)
                hash_node->max = rb_findMax_REPORT(*(hash_node->report_rb_pointer));
        }
    }
}

void delent_recursive(rbTree rel_rb, char *entID)
{
    struct hashtable_node *hash_node;
    rbNode_RELSHIP rb_relship_node;
    rbNode_REPORT rb_report_node;
    rbNode_RELSHIP toFree;
    if (rel_rb == NIL)
        return;
    delent_recursive(rel_rb->left, entID);

    hash_node = hashtable_search(rel_hashtable, rel_rb->key);
    rb_report_node = rb_search_REPORT(*(hash_node->report_rb_pointer), entID);
    for (; rb_report_node->count != 0; rb_report_node->count--)
    {
        rb_relship_node = rb_searchTo_RELSHIP(*(hash_node->relship_rb_pointer), entID);
        toFree = rb_remove_RELSHIP(hash_node->relship_rb_pointer, rb_relship_node);
        free(toFree);
    }
    if (rb_report_node->count == hash_node->max)
        hash_node->max = rb_findMax_REPORT(*(hash_node->report_rb_pointer));

    rb_relship_node = rb_searchFrom_RELSHIP(*(hash_node->relship_rb_pointer), entID);
    while (rb_relship_node != NIL_RELSHIP)
    {
        rb_report_node = rb_search_REPORT(*(hash_node->report_rb_pointer), rb_To_RELSHIP(*(hash_node->relship_rb_pointer), rb_relship_node));
        rb_report_node->count--;
        toFree = rb_remove_RELSHIP(hash_node->relship_rb_pointer, rb_relship_node);
        free(toFree);
        if (rb_report_node->count + 1 == hash_node->max)
            hash_node->max = rb_findMax_REPORT(*(hash_node->report_rb_pointer));
        rb_relship_node = rb_searchFrom_RELSHIP(*(hash_node->relship_rb_pointer), entID);
    }
    delent_recursive(rel_rb->right, entID);
    return;
}

void delent(char *entID)
{
    //printf("Deleting ent %s\n", entID);
    rbNode ent_node = rb_search(ent_rb, entID);
    if (ent_node == NIL)
    {
        //printf("Not deleting ent %s\n", entID);
        return;
    }
    rbNode toFree = rb_remove(&ent_rb, ent_node);
    free(toFree);
    delent_recursive(rel_rb, entID);
    return;
}

void report()
{
    if (rel_rb == NIL)
    {
        fputs("none\n", stdout);
        return;
    }
    report_recursive(rel_rb);
    fputs("\n", stdout);
}

void lineParser(char *temp)
{
    char *punt;
    punt = strtok(temp, " ");

    if (strcmp(punt, "addent") == 0)
    {
        char *entID;
        punt = strtok(NULL, " ");
        entID = strtok(punt, "\"");
        addent(entID);
    }

    if (strcmp(punt, "addrel") == 0)
    {
        char *entID1;
        char *entID2;
        char *relID;
        entID1 = strtok(NULL, "\"");
        punt = strtok(NULL, "\"");
        entID2 = strtok(NULL, "\"");
        punt = strtok(NULL, "\"");
        relID = strtok(NULL, "\"");
        addrel(relID, entID1, entID2);
    }

    if (strcmp(punt, "delrel") == 0)
    {
        char *entID1;
        char *entID2;
        char *relID;
        entID1 = strtok(NULL, "\"");
        punt = strtok(NULL, "\"");
        entID2 = strtok(NULL, "\"");
        punt = strtok(NULL, "\"");
        relID = strtok(NULL, "\"");
        delrel(relID, entID1, entID2);
    }

    if (strcmp(punt, "delent") == 0)
    {
        char *entID;
        punt = strtok(NULL, " ");
        entID = strtok(punt, "\"");
        delent(entID);
    }

    if (strcmp(punt, "report\n") == 0)
    {
        report();
    }
    return;
}

int main()
{
    //freopen("generator_suite/i/2019_08_10_11_21_21_790.txt", "r", stdin);
    freopen("suite2/batch2.2.in", "r", stdin);
    //freopen("output.txt", "w", stdout);

    rel_hashtable = hashtable_createTable(REL_HASH_SIZE);

    char temp[LINE_SIZE];
    fgets(temp, LINE_SIZE, stdin);
    for (; strcmp(temp, "end\n") != 0;)
    {
        lineParser(temp);
        fgets(temp, LINE_SIZE, stdin);
    }
    printf("%s", rb_searchFrom_RELSHIP(*(hashtable_search(rel_hashtable, "older_than")->relship_rb_pointer), "Airiam")->key);
    //rb_inOrder_RELSHIP(*(hashtable_search(rel_hashtable, "knows")->relship_rb_pointer));
    //printf("\n");
    //rb_inOrder_REPORT(*(hashtable_search(rel_hashtable, "knows")->report_rb_pointer));
    return 0;
}
