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
typedef struct rb_node_DEST *rbNode_DEST;
typedef struct rb_node_DEST *rbTree_DEST;

struct hashtable *rel_hashtable = NULL;

struct hashtable_node
{
    char key[REL_ID_SIZE];
    rbTree_DEST *val;
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
        z->value = y->value;
        z->count = y->count;
    }
    if (y->color == BLACK)
    {
        rb_removeFixUp_DEST(tree, x);
    }
    return y;
}

void rb_destroy_DEST(rbTree_DEST tree)
{
    if (tree == NIL_DEST)
        return;
    rb_destroy_DEST(tree->left);
    rb_destroy_DEST(tree->right);
    free(tree);
}

void rb_inOrder_DEST(rbTree_DEST tree)
{
    if (tree == NIL_DEST)
        return;

    rb_inOrder_DEST(tree->left);
    printf("### %s with %d ###", tree->key, tree->count);
    rb_inOrder_DEST(tree->right);
}

int rb_findMax_DEST(rbTree_DEST tree)
{
    int max;
    int left;
    int right;
    if (tree == NIL_DEST)
        return -1;

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

void print_max(rbTree_DEST root, int max)
{
    if (root == NULL)
        return;
    print_max(root->left, max);
    //printf("# %d #", root->count);
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
    struct hashtable_node *node;
    if (rel_rb == NIL)
        return;
    report_recursive2(rel_rb->left);
    node = hashtable_search(rel_hashtable, rel_rb->key);
    if (node != NULL && node->max > 0)
    {
        fputs("\"", stdout);
        fputs(rel_rb->key, stdout);
        fputs("\" ", stdout);
        print_max(*(node->val), node->max);
        printf("%d", node->max);
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
    struct hashtable_node *hash_node;
    rbTree_DEST *buffer;
    rbTree_DEST rel_dest_node;
    hash_node = hashtable_search(rel_hashtable, relID);
    if (hash_node == NULL)
    {
        buffer = malloc(sizeof(void *));
        *buffer = NIL_DEST;
        hashtable_insert(rel_hashtable, relID, buffer, 0);
    }
    hash_node = hashtable_search(rel_hashtable, relID);
    rb_insert(&rel_rb, relID);
    rel_dest_node = rb_search_DEST(*(hash_node->val), entID2);
    if (rel_dest_node == NIL_DEST)
    {
        rb_insert_DEST(hash_node->val, entID2, NIL);
        rel_dest_node = rb_search_DEST(*(hash_node->val), entID2);
    }

    if (rb_insert(&(rel_dest_node->value), entID1) == 1)
    {
        rel_dest_node->count++;
        if (rel_dest_node->count > hash_node->max)
            hash_node->max = rel_dest_node->count;
    }
    return;
}

void delent_destTreeCleaner_recursive(rbTree_DEST *rel_dest_tree, rbNode_DEST rel_dest_node)
{
    if (rel_dest_node == NIL_DEST)
        return;
    rbNode_DEST toFree;
    if (rel_dest_node->count < 1)
    {
        toFree = rb_remove_DEST(rel_dest_tree, rel_dest_node);
        free(toFree);
        delent_destTreeCleaner_recursive(rel_dest_tree, *rel_dest_tree);
        return;
    }
    delent_destTreeCleaner_recursive(rel_dest_tree, rel_dest_node->left);
    delent_destTreeCleaner_recursive(rel_dest_tree, rel_dest_node->right);
    return;
}

void delrel(char *relID, char *entID1, char *entID2)
{
    //printf("Deleting relationship %s between %s and %s\n", relID, entID1, entID2);
    struct hashtable_node *hash_node;
    rbTree_DEST *rel_dest_tree;
    rbTree_DEST rel_dest_node;
    if (rb_search(ent_rb, entID1) == NIL)
        return;

    hash_node = hashtable_search(rel_hashtable, relID);
    if (hash_node == NULL)
    {
        //printf("Relationship %s between %s and %s not deleted!", relID, entID1, entID2);
        return;
    }
    rel_dest_tree = hash_node->val;
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
    (rel_dest_node->count)--;
    if ((rel_dest_node->count + 1) == hash_node->max)
    {
        hash_node->max = rb_findMax_DEST(*rel_dest_tree);
    }
    if (rel_dest_node->count == 0)
    {
        rbNode_DEST toFree_DEST = rb_remove_DEST(rel_dest_tree, rel_dest_node);
        free(toFree_DEST);
        hash_node->max = rb_findMax_DEST(*rel_dest_tree);
    }
    if (hash_node->max == -1)
    {
        rb_destroy_DEST(*rel_dest_tree);
        *rel_dest_tree = NIL_DEST;
    }
    return;
}

void delent_relTreeCleaner_recursive(rbTree *rel_rb, rbNode rel_node)
{
    if (rel_node == NIL)
        return;
    rbNode toFree;
    if (hashtable_search(rel_hashtable, rel_node->key)->max < 1)
    {
        //printf("### Going to free: %s", rel_node->key);
        toFree = rb_remove(rel_rb, rel_node);
        free(toFree);
        delent_relTreeCleaner_recursive(rel_rb, *rel_rb);
        return;
    }
    delent_relTreeCleaner_recursive(rel_rb, rel_node->left);
    delent_relTreeCleaner_recursive(rel_rb, rel_node->right);
    return;
}

void delent_DESTnodes_recursive(rbTree_DEST *rel_dest_tree, rbNode_DEST rel_dest_node, char *entID, char *relID, struct hashtable_node *hash_node)
{
    if (rel_dest_node == NIL_DEST)
        return;
    delent_DESTnodes_recursive(rel_dest_tree, rel_dest_node->left, entID, relID, hash_node);

    if (strcmp(rel_dest_node->key, entID) == 0)
    {
        rb_destroy(rel_dest_node->value);
        rel_dest_node->value = NIL;
        //printf("\n###Deleting %s --RELDESTNODEVALUE: %d, HASHMAX: %d\n", entID, rel_dest_node->count, *hashtable_max);
        if ((rel_dest_node->count) == hash_node->max)
        {
            rel_dest_node->count = 0;
            hash_node->max = rb_findMax_DEST(*rel_dest_tree);
        }
        rel_dest_node->count = 0;
    }
    else
    {
        rbNode rel_from_node = rb_search(rel_dest_node->value, entID);
        if (rel_from_node != NIL)
        {
            //printf("\nDeleting from %s to %s\n", rel_from_node->key, rel_dest_node->key);
            rbNode toFree = rb_remove(&(rel_dest_node->value), rel_from_node);
            free(toFree);
            rel_dest_node->count--;
            if (((rel_dest_node->count) + 1) == hash_node->max)
                hash_node->max = rb_findMax_DEST(*rel_dest_tree);
        }
    }

    delent_DESTnodes_recursive(rel_dest_tree, rel_dest_node->right, entID, relID, hash_node);
    return;
}

void delent_recursive(rbTree rel_rb, char *entID)
{
    struct hashtable_node *hash_node;
    rbTree_DEST *rel_dest_tree;
    if (rel_rb == NIL)
        return;
    delent_recursive(rel_rb->left, entID);

    hash_node = hashtable_search(rel_hashtable, rel_rb->key);
    if (hash_node != NULL)
    {
        rel_dest_tree = hash_node->val;
        if (*rel_dest_tree != NIL_DEST)
        {
            delent_DESTnodes_recursive(rel_dest_tree, *rel_dest_tree, entID, rel_rb->key, hash_node);
            if (hash_node->max == 0)
            {
                rb_destroy_DEST(*rel_dest_tree);
                *rel_dest_tree = NIL_DEST;
            }
        }
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

    delent_relTreeCleaner_recursive(&rel_rb, rel_rb);
    return;
}

void report()
{
    if (rel_rb == NIL)
    {
        fputs("none\n", stdout);
        return;
    }
    report_recursive2(rel_rb);
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
    freopen("generator_suite/i_bis/2019_08_10_22_20_11_302.txt", "r", stdin);
    //freopen("suite4/batch4.2.in", "r", stdin);
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
