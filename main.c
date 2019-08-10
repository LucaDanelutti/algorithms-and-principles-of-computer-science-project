#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define REL_HASH_SIZE 2
#define REL_ID_SIZE 20 //Poni uguale a ENT_ID_SIZE
#define ENT_ID_SIZE 20
#define LINE_SIZE 100

const int RED = 0;
const int BLACK = 1;

typedef struct rb_node *rbNode;
typedef struct rb_node *rbTree;
typedef struct rb_node_DEST *rbNode_DEST;
typedef struct rb_node_DEST *rbTree_DEST;

struct rb_node_REPORT *report_rb = NULL;
struct hashtable *rel_hashtable = NULL;

struct hashtable_node
{
    char key[REL_ID_SIZE];
    void *val;
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

void hashtable_insert(struct hashtable *t, char *key, void *val, int max)
{
    int pos = hashtable_hashFunction(t, key);
    struct hashtable_node *list = t->list[pos];
    struct hashtable_node *newNode = (struct hashtable_node *)malloc(sizeof(struct hashtable_node));
    struct hashtable_node *temp = list;
    while (temp)
    {
        if (temp->key == key)
        {
            temp->val = val;
            return;
        }
        temp = temp->next;
    }
    strcpy(newNode->key, key);
    newNode->val = val;
    newNode->max = max;
    newNode->next = list;
    t->list[pos] = newNode;
}

rbTree_DEST *hashtable_search(struct hashtable *t, char *key)
{
    int pos = hashtable_hashFunction(t, key);
    struct hashtable_node *list = t->list[pos];
    struct hashtable_node *temp = list;
    while (temp)
    {
        if (!strcmp(temp->key, key))
        {
            return temp->val;
        }
        temp = temp->next;
    }
    return NULL;
}

int *hashtable_max_search(struct hashtable *t, char *key)
{
    int pos = hashtable_hashFunction(t, key);
    struct hashtable_node *list = t->list[pos];
    struct hashtable_node *temp = list;
    while (temp)
    {
        if (!strcmp(temp->key, key))
        {
            return &(temp->max);
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

void rb_insert(rbTree *tree, char *k)
{
    rbNode y = NIL;
    rbNode x = *tree;
    rbNode z = (struct rb_node *)malloc(sizeof(struct rb_node));
    strcpy(z->key, k);
    z->parent = z->left = z->right = NIL;

    while (x != NIL)
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

void rb_inOrder(rbTree tree)
{
    if (tree == NIL)
        return;

    rb_inOrder(tree->left);
    printf("%s ", tree->key);
    rb_inOrder(tree->right);
}

//############
//RB-TREE DEST
//############

struct rb_node_DEST
{
    char key[ENT_ID_SIZE];
    rbTree value;
    int count;
    int color;
    rbNode_DEST parent, left, right;
};

static struct rb_node_DEST sentinel_DEST = {"0", NIL, 0, 1, 0, 0, 0};
#define NIL_DEST &sentinel_DEST

rbNode_DEST rb_search_DEST(rbTree_DEST tree, char *k)
{
    if (tree == NIL_DEST || strcmp(k, tree->key) == 0)
    {
        return tree;
    }
    if (strcmp(k, tree->key) < 0)
    {
        return rb_search_DEST(tree->left, k);
    }
    else
    {
        return rb_search_DEST(tree->right, k);
    }
}

rbNode_DEST rb_minimum_DEST(rbTree_DEST tree)
{
    while (tree->left != NIL_DEST)
    {
        tree = tree->left;
    }
    return tree;
}

rbNode_DEST rb_maximum_DEST(rbTree_DEST tree)
{
    while (tree->right != NIL_DEST)
    {
        tree = tree->right;
    }
    return tree;
}

rbNode_DEST rb_successor_DEST(rbNode_DEST x)
{
    if (x->right != NIL_DEST)
    {
        return rb_minimum_DEST(x->right);
    }
    rbNode_DEST y = x->parent;
    while (y != NIL_DEST && x == y->right)
    {
        x = y;
        y = y->parent;
    }
    return y;
}

void rb_leftRotate_DEST(rbTree_DEST *tree, rbNode_DEST x)
{
    rbNode_DEST y = x->right;
    x->right = y->left;
    if (y->left != NIL_DEST)
    {
        y->left->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == NIL_DEST)
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

void rb_rightRotate_DEST(rbTree_DEST *tree, rbNode_DEST x)
{
    rbNode_DEST y = x->left;
    x->left = y->right;
    if (y->right != NIL_DEST)
    {
        y->right->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == NIL_DEST)
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

void rb_insertFixUp_DEST(rbTree_DEST *tree, rbNode_DEST z)
{
    while (z->parent->color == RED)
    {
        if (z->parent == z->parent->parent->left)
        {
            rbNode_DEST y = z->parent->parent->right;
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
                    rb_leftRotate_DEST(tree, z); //case2->case3
                }
                z->parent->color = BLACK; //Case3
                z->parent->parent->color = RED;
                rb_rightRotate_DEST(tree, z->parent->parent);
            }
        }
        else
        {
            rbNode_DEST y = z->parent->parent->left;
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
                    rb_rightRotate_DEST(tree, z); //case2->case3
                }
                z->parent->color = BLACK; //Case3
                z->parent->parent->color = RED;
                rb_leftRotate_DEST(tree, z->parent->parent);
            }
        }
    }
    (*tree)->color = BLACK;
}

void rb_insert_DEST(rbTree_DEST *tree, char *k, rbTree value)
{
    rbNode_DEST y = NIL_DEST;
    rbNode_DEST x = *tree;
    rbNode_DEST z = (struct rb_node_DEST *)malloc(sizeof(struct rb_node_DEST));
    strcpy(z->key, k);
    z->value = value;
    z->count = 0;
    z->parent = z->left = z->right = NIL_DEST;

    while (x != NIL_DEST)
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
    if (y == NIL_DEST)
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
    z->left = z->right = NIL_DEST;
    z->color = RED;
    rb_insertFixUp_DEST(tree, z);
}

void rb_removeFixUp_DEST(rbTree_DEST *tree, rbNode_DEST x)
{
    while (x != *tree && x->color == BLACK)
    {
        if (x == x->parent->left)
        {
            rbNode_DEST w = x->parent->right;
            if (w->color == RED)
            { //Case1
                w->color = BLACK;
                x->parent->color = RED;
                rb_leftRotate_DEST(tree, x->parent);
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
                rb_rightRotate_DEST(tree, w);
                w = x->parent->right;
            }
            else
            { //Case4
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->right->color = BLACK;
                rb_leftRotate_DEST(tree, x->parent);
                x = *tree;
            }
        }
        else
        {
            rbNode_DEST w = x->parent->left;
            if (w->color == RED)
            { //Case1
                w->color = BLACK;
                x->parent->color = RED;
                rb_rightRotate_DEST(tree, x->parent);
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
                rb_leftRotate_DEST(tree, w);
                w = x->parent->left;
            }
            else
            { //Case4
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->left->color = BLACK;
                rb_rightRotate_DEST(tree, x->parent);
                x = *tree;
            }
        }
    }
    x->color = BLACK;
}

rbNode_DEST rb_remove_DEST(rbTree_DEST *tree, rbNode_DEST z)
{
    rbNode_DEST x, y;
    if (z->left == NIL_DEST || z->right == NIL_DEST)
    {
        y = z;
    }
    else
    {
        y = rb_successor_DEST(z);
    }
    if (y->left != NIL_DEST)
    {
        x = y->left;
    }
    else
    {
        x = y->right;
    }
    x->parent = y->parent;
    if (y->parent == NIL_DEST)
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
        rb_removeFixUp_DEST(tree, x);
    }
    return y;
}

void rb_inOrder_DEST(rbTree_DEST tree)
{
    if (tree == NIL_DEST)
        return;

    rb_inOrder_DEST(tree->left);
    printf("%s ", tree->key);
    rb_inOrder_DEST(tree->right);
}

int rb_findMax_DEST(rbTree_DEST tree)
{
    int max;
    int left;
    int right;
    if (tree == NIL_DEST)
        return 0;

    left = rb_findMax_DEST(tree->left);
    right = rb_findMax_DEST(tree->right);
    if (tree->count > left)
        max = tree->count;
    else
        max = left;
    if (max > right)
        return max;
    return right;
}

struct rb_node_REPORT
{
    int key;
    char dest[ENT_ID_SIZE];
    char color;
    struct rb_node_REPORT *left, *right, *parent;
};

void rb_leftRotate_REPORT(struct rb_node_REPORT **root, struct rb_node_REPORT *x)
{
    if (!x || !x->right)
        return;
    struct rb_node_REPORT *y = x->right;
    x->right = y->left;
    if (x->right != NULL)
        x->right->parent = x;
    y->parent = x->parent;
    if (x->parent == NULL)
        (*root) = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;
    y->left = x;
    x->parent = y;
}

void rb_rightRotate_REPORT(struct rb_node_REPORT **root, struct rb_node_REPORT *y)
{
    if (!y || !y->left)
        return;
    struct rb_node_REPORT *x = y->left;
    y->left = x->right;
    if (x->right != NULL)
        x->right->parent = y;
    x->parent = y->parent;
    if (x->parent == NULL)
        (*root) = x;
    else if (y == y->parent->left)
        y->parent->left = x;
    else
        y->parent->right = x;
    x->right = y;
    y->parent = x;
}

void rb_insertFixUp_REPORT(struct rb_node_REPORT **root, struct rb_node_REPORT *z)
{
    while (z != *root && z != (*root)->left && z != (*root)->right && z->parent->color == 'R')
    {
        struct rb_node_REPORT *y;

        if (z->parent && z->parent->parent && z->parent == z->parent->parent->left)
            y = z->parent->parent->right;
        else
            y = z->parent->parent->left;

        if (!y)
            z = z->parent->parent;
        else if (y->color == 'R')
        {
            y->color = 'B';
            z->parent->color = 'B';
            z->parent->parent->color = 'R';
            z = z->parent->parent;
        }

        else
        {
            if (z->parent == z->parent->parent->left &&
                z == z->parent->left)
            {
                char ch = z->parent->color;
                z->parent->color = z->parent->parent->color;
                z->parent->parent->color = ch;
                rb_rightRotate_REPORT(root, z->parent->parent);
            }

            if (z->parent && z->parent->parent && z->parent == z->parent->parent->left &&
                z == z->parent->right)
            {
                char ch = z->color;
                z->color = z->parent->parent->color;
                z->parent->parent->color = ch;
                rb_leftRotate_REPORT(root, z->parent);
                rb_rightRotate_REPORT(root, z->parent->parent);
            }

            if (z->parent && z->parent->parent &&
                z->parent == z->parent->parent->right &&
                z == z->parent->right)
            {
                char ch = z->parent->color;
                z->parent->color = z->parent->parent->color;
                z->parent->parent->color = ch;
                rb_leftRotate_REPORT(root, z->parent->parent);
            }

            if (z->parent && z->parent->parent && z->parent == z->parent->parent->right &&
                z == z->parent->left)
            {
                char ch = z->color;
                z->color = z->parent->parent->color;
                z->parent->parent->color = ch;
                rb_rightRotate_REPORT(root, z->parent);
                rb_leftRotate_REPORT(root, z->parent->parent);
            }
        }
    }
    (*root)->color = 'B';
}

void rb_insert_REPORT(struct rb_node_REPORT **root, int key, char *dest)
{
    struct rb_node_REPORT *z = (struct rb_node_REPORT *)malloc(sizeof(struct rb_node_REPORT));
    z->key = key;
    strcpy(z->dest, dest);
    z->left = z->right = z->parent = NULL;

    if (*root == NULL)
    {
        z->color = 'B';
        (*root) = z;
    }
    else
    {
        struct rb_node_REPORT *y = NULL;
        struct rb_node_REPORT *x = (*root);

        while (x != NULL)
        {
            y = x;
            if (z->key < x->key)
                x = x->left;
            else
                x = x->right;
        }
        z->parent = y;
        if (z->key >= y->key)
            y->right = z;
        else
            y->left = z;
        z->color = 'R';

        rb_insertFixUp_REPORT(root, z);
    }
}

struct rb_node_REPORT *rb_search_REPORT(struct rb_node_REPORT *root, int key)
{
    if (root == NULL)
        return NULL;
    if (root->key == key)
        return root;
    if (root->key > key)
        return rb_search_REPORT(root->left, key);
    else
        return rb_search_REPORT(root->right, key);
}

void rb_inorder_REPORT(struct rb_node_REPORT *root)
{
    if (root == NULL)
        return;
    rb_inorder_REPORT(root->left);
    printf("%d to %s; ", root->key, root->dest);
    rb_inorder_REPORT(root->right);
}

void rb_destroy_REPORT(struct rb_node_REPORT *root)
{
    if (root == NULL)
        return;
    rb_destroy_REPORT(root->left);
    rb_destroy_REPORT(root->right);
    free(root);
}

void print_max(rbTree_DEST root, int max)
{
    if (root == NULL)
        return;
    print_max(root->left, max);
    if (root->count == max)
    {
        fputs("\"", stdout);
        fputs(root->key, stdout);
        fputs("\" ", stdout);
    }
    print_max(root->right, max);
}

void report_recursive2(rbTree rel_rb)
{
    int max;
    rbTree_DEST *node;
    if (rel_rb == NIL)
        return;
    report_recursive2(rel_rb->left);
    node = hashtable_search(rel_hashtable, rel_rb->key);
    max = *hashtable_max_search(rel_hashtable, rel_rb->key);
    if (node != NULL)
    {
        fputs("\"", stdout);
        fputs(rel_rb->key, stdout);
        fputs("\" ", stdout);
        print_max(*node, max);
        printf("%d", max);
        fputs("; ", stdout);
    }
    report_recursive2(rel_rb->right);
    return;
}

void addent(char *entID)
{
    //printf("Adding ent %s\n", entID);
    if (rb_search(ent_rb, entID) == NIL)
        rb_insert(&ent_rb, entID);
}

void addrel(char *relID, char *entID1, char *entID2)
{
    if (rb_search(ent_rb, entID1) == NIL || rb_search(ent_rb, entID2) == NIL)
    {
        //printf("Relationship %s between %s and %s not added!", relID, entID1, entID2);
        return;
    }
    //printf("Adding relationship %s between %s and %s\n", relID, entID1, entID2);
    rbTree_DEST *buffer;
    rbTree_DEST rel_dest_node;
    buffer = hashtable_search(rel_hashtable, relID);
    if (buffer == NULL)
    {
        buffer = malloc(sizeof(void *));
        *buffer = NIL_DEST;
        hashtable_insert(rel_hashtable, relID, buffer, 0);
        rb_insert(&rel_rb, relID);
    }
    rel_dest_node = rb_search_DEST(*buffer, entID2);
    if (rel_dest_node == NIL_DEST)
        rb_insert_DEST(buffer, entID2, NIL);
    rel_dest_node = rb_search_DEST(*buffer, entID2);
    if (rb_search(rel_dest_node->value, entID1) == NIL)
    {
        rel_dest_node->count++;
        rb_insert(&(rel_dest_node->value), entID1);
        if (rel_dest_node->count > *hashtable_max_search(rel_hashtable, relID))
            *hashtable_max_search(rel_hashtable, relID) = rel_dest_node->count;
    }
    return;
}

void delrel(char *relID, char *entID1, char *entID2)
{
    //printf("Deleting relationship %s between %s and %s\n", relID, entID1, entID2);
    rbTree_DEST *rel_dest_tree;
    rbTree_DEST rel_dest_node;
    rel_dest_tree = hashtable_search(rel_hashtable, relID);
    if (rel_dest_tree == NULL)
    {
        //printf("Relationship %s between %s and %s not deleted!", relID, entID1, entID2);
        return;
    }
    if (*rel_dest_tree == NIL_DEST)
    {
        //printf("Relationship %s between %s and %s not deleted!", relID, entID1, entID2);
        return;
    }
    rel_dest_node = rb_search_DEST(*rel_dest_tree, entID2);
    if (rel_dest_node == NIL_DEST)
    {
        //printf("Relationship %s between %s and %s not deleted!", relID, entID1, entID2);
        return;
    }
    rbNode rel_from_node = rb_search(rel_dest_node->value, entID1);
    if (rel_from_node == NIL)
    {
        //printf("Relationship %s between %s and %s not deleted!", relID, entID1, entID2);
        return;
    }

    rbNode toFree = rb_remove(&(rel_dest_node->value), rel_from_node);
    free(toFree);
    int *hashtable_max = hashtable_max_search(rel_hashtable, relID);
    rel_dest_node->count--;
    if (rel_dest_node->count == 0)
    {
        rbNode_DEST toFree_DEST = rb_remove_DEST(rel_dest_tree, rel_dest_node);
        free(toFree_DEST);
    }
    if ((rel_dest_node->count + 1) == *hashtable_max)
        *hashtable_max = rb_findMax_DEST(*rel_dest_tree);
    return;
}

void delent(char *entID)
{
    //printf("Deleting ent %s\n", entID);
    rbNode ent_node = rb_search(ent_rb, entID);
    if (ent_node == NIL)
    {
        return;
    }
    rb_remove(&ent_rb, ent_node);

    return;
}

void report()
{
    if (rel_rb == NIL)
    {
        fputs("none\n", stdout);
        return;
    }
    //report_recursive(rel_rb);
    report_recursive2(rel_rb);
    fputs("\n", stdout);
}

void lineParser(char *temp)
{
    char *punt;
    punt = strtok(temp, " ");

    if (strcmp(punt, "addent") == 0)
    {
        char entID[ENT_ID_SIZE];
        punt = strtok(NULL, " ");
        punt = strtok(punt, "\"");
        strcpy(entID, punt);
        addent(entID);
    }

    if (strcmp(punt, "addrel") == 0)
    {
        char entID1[ENT_ID_SIZE];
        char entID2[ENT_ID_SIZE];
        char relID[REL_ID_SIZE];
        punt = strtok(NULL, "\"");
        strcpy(entID1, punt);
        punt = strtok(NULL, "\"");
        punt = strtok(NULL, "\"");
        strcpy(entID2, punt);
        punt = strtok(NULL, "\"");
        punt = strtok(NULL, "\"");
        strcpy(relID, punt);
        addrel(relID, entID1, entID2);
    }

    if (strcmp(punt, "delrel") == 0)
    {
        char entID1[ENT_ID_SIZE];
        char entID2[ENT_ID_SIZE];
        char relID[REL_ID_SIZE];
        punt = strtok(NULL, "\"");
        strcpy(entID1, punt);
        punt = strtok(NULL, "\"");
        punt = strtok(NULL, "\"");
        strcpy(entID2, punt);
        punt = strtok(NULL, "\"");
        punt = strtok(NULL, "\"");
        strcpy(relID, punt);
        delrel(relID, entID1, entID2);
    }

    if (strcmp(punt, "report\n") == 0)
    {
        report();
    }
    return;
}

int main()
{
    freopen("generator_suite/i/2019_08_10_11_21_19_396.txt", "r", stdin);
    //freopen("suite1/batch1.1.in", "r", stdin);
    freopen("output.txt", "w", stdout);

    rel_hashtable = hashtable_createTable(REL_HASH_SIZE);

    char temp[LINE_SIZE];
    fgets(temp, LINE_SIZE, stdin);
    for (; strcmp(temp, "end\n") != 0;)
    {
        lineParser(temp);
        fgets(temp, LINE_SIZE, stdin);
    }
    return 0;
}
