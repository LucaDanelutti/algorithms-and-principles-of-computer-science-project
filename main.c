#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define REL_HASH_SIZE 2
#define REL_ID_SIZE 100
#define ENT_ID_SIZE 100
#define LINE_SIZE 1000

struct rb_node *ent_rb = NULL;
struct rb_node *rel_rb = NULL;
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

struct rb_node_DEST **hashtable_search(struct hashtable *t, char *key)
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

struct rb_node
{
    char key[ENT_ID_SIZE];
    char color;
    struct rb_node *left, *right, *parent;
};

void rb_leftRotate(struct rb_node **root, struct rb_node *x)
{
    if (!x || !x->right)
        return;
    struct rb_node *y = x->right;
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

void rb_rightRotate(struct rb_node **root, struct rb_node *y)
{
    if (!y || !y->left)
        return;
    struct rb_node *x = y->left;
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

void rb_insertFixUp(struct rb_node **root, struct rb_node *z)
{
    while (z != *root && z != (*root)->left && z != (*root)->right && z->parent->color == 'R')
    {
        struct rb_node *y;

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
                rb_rightRotate(root, z->parent->parent);
            }

            if (z->parent && z->parent->parent && z->parent == z->parent->parent->left &&
                z == z->parent->right)
            {
                char ch = z->color;
                z->color = z->parent->parent->color;
                z->parent->parent->color = ch;
                rb_leftRotate(root, z->parent);
                rb_rightRotate(root, z->parent->parent);
            }

            if (z->parent && z->parent->parent &&
                z->parent == z->parent->parent->right &&
                z == z->parent->right)
            {
                char ch = z->parent->color;
                z->parent->color = z->parent->parent->color;
                z->parent->parent->color = ch;
                rb_leftRotate(root, z->parent->parent);
            }

            if (z->parent && z->parent->parent && z->parent == z->parent->parent->right &&
                z == z->parent->left)
            {
                char ch = z->color;
                z->color = z->parent->parent->color;
                z->parent->parent->color = ch;
                rb_rightRotate(root, z->parent);
                rb_leftRotate(root, z->parent->parent);
            }
        }
    }
    (*root)->color = 'B';
}

void rb_insert(struct rb_node **root, char *key)
{
    struct rb_node *z = (struct rb_node *)malloc(sizeof(struct rb_node));
    strcpy(z->key, key);
    z->left = z->right = z->parent = NULL;

    if (*root == NULL)
    {
        z->color = 'B';
        (*root) = z;
    }
    else
    {
        struct rb_node *y = NULL;
        struct rb_node *x = (*root);

        while (x != NULL)
        {
            y = x;
            if (strcmp(z->key, x->key) < 0)
                x = x->left;
            else
                x = x->right;
        }
        z->parent = y;
        if (strcmp(z->key, y->key) > 0)
            y->right = z;
        else
            y->left = z;
        z->color = 'R';

        rb_insertFixUp(root, z);
    }
}

struct rb_node *rb_search(struct rb_node *root, char *key)
{
    if (root == NULL)
        return NULL;
    if (strcmp(root->key, key) == 0)
        return root;
    if (strcmp(root->key, key) > 0)
        return rb_search(root->left, key);
    else
        return rb_search(root->right, key);
}

void rb_inorder(struct rb_node *root)
{
    if (root == NULL)
        return;
    rb_inorder(root->left);
    printf("%s ", root->key);
    rb_inorder(root->right);
}

int rb_count_recursive(struct rb_node *root)
{
    int count = 1;
    if (root->left != NULL)
        count += rb_count_recursive(root->left);
    if (root->right != NULL)
        count += rb_count_recursive(root->right);
    return count;
}

int rb_count(struct rb_node *root)
{
    int count = 0;
    if (root != NULL)
        rb_count_recursive(root);
    return count;
}

struct rb_node_DEST
{
    char key[ENT_ID_SIZE];
    struct rb_node *value;
    int count;
    char color;
    struct rb_node_DEST *left, *right, *parent;
};

void rb_leftRotate_DEST(struct rb_node_DEST **root, struct rb_node_DEST *x)
{
    if (!x || !x->right)
        return;
    struct rb_node_DEST *y = x->right;
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

void rb_rightRotate_DEST(struct rb_node_DEST **root, struct rb_node_DEST *y)
{
    if (!y || !y->left)
        return;
    struct rb_node_DEST *x = y->left;
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

void rb_insertFixUp_DEST(struct rb_node_DEST **root, struct rb_node_DEST *z)
{
    while (z != *root && z != (*root)->left && z != (*root)->right && z->parent->color == 'R')
    {
        struct rb_node_DEST *y;

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
                rb_rightRotate_DEST(root, z->parent->parent);
            }

            if (z->parent && z->parent->parent && z->parent == z->parent->parent->left &&
                z == z->parent->right)
            {
                char ch = z->color;
                z->color = z->parent->parent->color;
                z->parent->parent->color = ch;
                rb_leftRotate_DEST(root, z->parent);
                rb_rightRotate_DEST(root, z->parent->parent);
            }

            if (z->parent && z->parent->parent &&
                z->parent == z->parent->parent->right &&
                z == z->parent->right)
            {
                char ch = z->parent->color;
                z->parent->color = z->parent->parent->color;
                z->parent->parent->color = ch;
                rb_leftRotate_DEST(root, z->parent->parent);
            }

            if (z->parent && z->parent->parent && z->parent == z->parent->parent->right &&
                z == z->parent->left)
            {
                char ch = z->color;
                z->color = z->parent->parent->color;
                z->parent->parent->color = ch;
                rb_rightRotate_DEST(root, z->parent);
                rb_leftRotate_DEST(root, z->parent->parent);
            }
        }
    }
    (*root)->color = 'B';
}

void rb_insert_DEST(struct rb_node_DEST **root, char *key, struct rb_node *value)
{
    struct rb_node_DEST *z = (struct rb_node_DEST *)malloc(sizeof(struct rb_node_DEST));
    strcpy(z->key, key);
    z->left = z->right = z->parent = NULL;
    z->value = value;
    z->count = 0;

    if (*root == NULL)
    {
        z->color = 'B';
        (*root) = z;
    }
    else
    {
        struct rb_node_DEST *y = NULL;
        struct rb_node_DEST *x = (*root);

        while (x != NULL)
        {
            y = x;
            if (strcmp(z->key, x->key) < 0)
                x = x->left;
            else
                x = x->right;
        }
        z->parent = y;
        if (strcmp(z->key, y->key) > 0)
            y->right = z;
        else
            y->left = z;
        z->color = 'R';

        rb_insertFixUp_DEST(root, z);
    }
}

struct rb_node_DEST *rb_search_DEST(struct rb_node_DEST *root, char *key)
{
    if (root == NULL)
        return NULL;
    if (strcmp(root->key, key) == 0)
        return root;
    if (strcmp(root->key, key) > 0)
        return rb_search_DEST(root->left, key);
    else
        return rb_search_DEST(root->right, key);
}

void rb_inorder_DEST(struct rb_node_DEST *root)
{
    if (root == NULL)
        return;
    rb_inorder_DEST(root->left);
    printf("%s with ", root->key);
    printf("%d relationships; ", root->count);
    rb_inorder_DEST(root->right);
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

void add_rel(char *relID, char *entID1, char *entID2)
{
    if (rb_search(ent_rb, entID1) == NULL || rb_search(ent_rb, entID2) == NULL)
        return;
    //printf("Adding relationship %s between %s and %s\n", relID, entID1, entID2);
    struct rb_node_DEST **buffer;
    struct rb_node_DEST *buffer2;
    buffer = hashtable_search(rel_hashtable, relID);
    if (buffer == NULL)
    {
        buffer = malloc(sizeof(void *));
        *buffer = NULL;
        hashtable_insert(rel_hashtable, relID, buffer, 0);
        rb_insert(&rel_rb, relID);
    }
    buffer2 = rb_search_DEST(*buffer, entID2);
    if (buffer2 == NULL)
        rb_insert_DEST(buffer, entID2, NULL);
    buffer2 = rb_search_DEST(*buffer, entID2);
    if (rb_search(buffer2->value, entID1) == NULL)
    {
        buffer2->count++;
        rb_insert(&(buffer2->value), entID1);
        if (buffer2->count > *hashtable_max_search(rel_hashtable, relID))
            *hashtable_max_search(rel_hashtable, relID) = buffer2->count;
    }
    return;
}

void print_max(struct rb_node_DEST *root, int max)
{
    if (root == NULL)
        return;
    print_max(root->left, max);
    if (root->count == max)
    {
        fputs("\"", stdout);
        fputs(root->key, stdout);
        fputs("\"", stdout);
    }
    print_max(root->right, max);
}

void build_report_rb(struct rb_node_DEST *root, int max)
{
    if (root == NULL)
        return;
    build_report_rb(root->right, max);
    if (root->count > max)
        max = root->count;
    if (root->count >= max)
        rb_insert_REPORT(&report_rb, root->count, root->key);
    //printf("Insert %s", root->key);
    build_report_rb(root->left, max);
}

int print_max_report_rb(struct rb_node_REPORT *root)
{
    int max;
    if (root->right == NULL)
    {
        fputs("\"", stdout);
        fputs(root->dest, stdout);
        fputs("\"", stdout);
        return root->key;
    }
    max = print_max_report_rb(root->right);
    if (max == root->key)
    {
        fputs(" \"", stdout);
        fputs(root->dest, stdout);
        fputs("\"", stdout);
    }
    return max;
}

void report_recursive(struct rb_node *rel_rb)
{
    struct rb_node_DEST **node;
    int max = 0;
    if (rel_rb == NULL)
        return;
    report_recursive(rel_rb->left);
    node = hashtable_search(rel_hashtable, rel_rb->key);
    if (node != NULL)
    {
        build_report_rb(*node, 0);
        fputs("\"", stdout);
        fputs(rel_rb->key, stdout);
        fputs("\" ", stdout);
        max = print_max_report_rb(report_rb);
        fputs(" ", stdout);
        printf("%d", max);
        fputs("; ", stdout);
        rb_destroy_REPORT(report_rb);
        report_rb = NULL;
    }
    report_recursive(rel_rb->right);
    return;
}

void report_recursive2(struct rb_node *rel_rb)
{
    int max;
    struct rb_node_DEST **node;
    if (rel_rb == NULL)
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
        fputs(" ", stdout);
        printf("%d", max);
        fputs("; ", stdout);
    }
    report_recursive2(rel_rb->right);
    return;
}

void report()
{
    //rb_inorder(rel_rb);
    if (rel_rb == NULL)
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
        //printf("Adding %s\n", entID);
        if (rb_search(ent_rb, entID) == NULL)
            rb_insert(&ent_rb, entID);
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
        add_rel(relID, entID1, entID2);
    }

    if (strcmp(punt, "report\n") == 0)
    {
        report();
    }
    return;
}

int main()
{
    freopen("suite/i/2019_08_09_15_06_15_549.txt", "r", stdin);
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
