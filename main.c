#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "raylib.h"
#include "raymath.h"
#include "util.h"

typedef struct {
    Vector2 p0, p1;
    Color c;
} Line;

typedef struct TreeNode TreeNode;
struct TreeNode {
    TreeNode *left, *right;
    Line line;
};

typedef struct {
    TreeNode **nodes;
    size_t cnt, cap;
} DATreeNodes;

static void da_init(DATreeNodes *da, size_t cap);
static void da_destroy(DATreeNodes *da);
static void da_append(DATreeNodes *da, TreeNode *node);
static TreeNode *da_pop(DATreeNodes *da);

static TreeNode *node_create(Line line);
static void tree_destroy(TreeNode *root);

static void generate_new_leaves(TreeNode *root, float angle, float scale);
static void draw_tree(TreeNode *root);

int
main(int argc, char *argv[])
{
    // initialize window

    const int sc_width = 800;
    const int sc_height = 600;

    InitWindow(sc_width, sc_height, "raylib test");
    SetTargetFPS(60);

    // generate tree

    Vector2 p0, p1;
    Line trunk;
    trunk.p0 = (Vector2){ (float)sc_width / 2.0, (float)sc_height / 8.0 };
    trunk.p1 = (Vector2){ (float)sc_width / 2.0, (float)sc_height / 4.0 };
    trunk.c = PURPLE;

    TreeNode *tree;
    tree = node_create(trunk);

    int n = 12;
    while (n--)
        generate_new_leaves(tree, PI/5, 0.7);

    // render loop

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawFPS(0, 0);

        draw_tree(tree);

        EndDrawing();
    }

    // cleanup

    tree_destroy(tree);
    CloseWindow();

    return 0;
}

TreeNode *
node_create(Line line)
{
    TreeNode *node;
    node = emalloc(sizeof(*node));
    node->line = line;
    node->left = node->right = NULL;
    return node;
}

void
da_init(DATreeNodes *da, size_t cap)
{
    da->cap = MAX(cap, 1);
    da->nodes = emalloc(da->cap * sizeof(TreeNode *));
    da->cnt = 0;
}

void
da_destroy(DATreeNodes *da)
{
    free(da->nodes);
    da->cnt = da->cnt = 0;
}

void
da_append(DATreeNodes *da, TreeNode *node)
{
    if (da->cnt == da->cap) {
        da->cap *= 2;
        da->nodes = erealloc(da->nodes, da->cap * sizeof(TreeNode *));
    }
    da->nodes[da->cnt++] = node;
}

TreeNode *
da_pop(DATreeNodes *da)
{
    return da->nodes[--da->cnt];
}

void
tree_destroy(TreeNode *root)
{
    DATreeNodes node_stack;

    assert(root);

    da_init(&node_stack, 1);
    da_append(&node_stack, root);
    while (node_stack.cnt > 0) {
        TreeNode *cur;
        cur = da_pop(&node_stack);
        if (cur->left)
            da_append(&node_stack, cur->left);
        if (cur->right)
            da_append(&node_stack, cur->right);
        free(cur);
    }

    da_destroy(&node_stack);
}

void
generate_new_leaves(TreeNode *root, float angle, float scale)
{
    DATreeNodes node_stack, leaf_stack;

    assert(root);

    da_init(&node_stack, 1);
    da_init(&leaf_stack, 1);

    da_append(&node_stack, root);
    while (node_stack.cnt > 0) {
        TreeNode *cur;
        cur = da_pop(&node_stack);
        if (!cur->left && !cur->right) {
            da_append(&leaf_stack, cur);
            continue;
        }
        if (cur->left)
            da_append(&node_stack, cur->left);
        if (cur->right)
            da_append(&node_stack, cur->right);
    }
    da_destroy(&node_stack);

    while (leaf_stack.cnt > 0) {
        TreeNode *cur;
        Line left_line, right_line;
        Vector2 left_vec, right_vec;

        cur = da_pop(&leaf_stack);

        left_vec = right_vec = Vector2Subtract(cur->line.p1, cur->line.p0);
        left_vec = right_vec = Vector2Scale(left_vec, scale);
        left_vec = Vector2Rotate(left_vec, -angle);
        right_vec = Vector2Rotate(right_vec, angle);

        left_line.p0 = cur->line.p1;
        left_line.p1 = Vector2Add(cur->line.p1, left_vec);
        right_line.p0 = cur->line.p1;
        right_line.p1 = Vector2Add(cur->line.p1, right_vec);

        left_line.c = right_line.c = cur->line.c;

        cur->left = node_create(left_line);
        cur->right = node_create(right_line);
    }
    da_destroy(&leaf_stack);
}

void
draw_tree(TreeNode *root)
{
    DATreeNodes node_stack;

    assert(root);

    da_init(&node_stack, 1);

    da_append(&node_stack, root);
    while (node_stack.cnt > 0) {
        TreeNode *cur;
        cur = da_pop(&node_stack);
        if (cur->left)
            da_append(&node_stack, cur->left);
        if (cur->right)
            da_append(&node_stack, cur->right);

        DrawLineV(cur->line.p0, cur->line.p1, cur->line.c);
    }
    da_destroy(&node_stack);
}

