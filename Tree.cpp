#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "Tree.h"
#include "TreeDump.h"

TreeNode* tree_node_new(Tree* tree, void* elem)
{
    assert(tree != nullptr);
    assert(elem != nullptr);

    TreeNode* node = (TreeNode*)calloc(1, sizeof(TreeNode));
    if(node == nullptr)
    {
        tree_status_errors(NODE_IS_NULL);
        return nullptr;
    }

    node->elem = tree->elem_ctor(elem);
    if(node->elem == nullptr)
    {
        tree_status_errors(ELEM_IS_NULL);
        return nullptr;
    }

    node->leftNode   = nullptr;
    node->rightNode  = nullptr;
    node->parentNode = nullptr; 
    
    if(tree->size == 0)
        tree->root = node;

    tree->size++;

    return node;
}

void tree_nodes_dtor(Tree* tree, TreeNode* node)
{
    assert(tree != nullptr);

    if(node == nullptr)
        return;

    tree_nodes_dtor(tree, node->leftNode);
    tree_nodes_dtor(tree, node->rightNode);
    tree->elem_dtor(node->elem);
    free(node);
}

Tree* tree_ctor(void* (*elem_ctor)(void* elem),
                void (*elem_dtor)(void* elem),
                void (*write_elem)(FILE* fp, void* elem))
{
    assert(elem_ctor != nullptr);
    assert(elem_dtor != nullptr);
    assert(write_elem != nullptr);

    Tree* tree = (Tree*)calloc(1, sizeof(Tree));
    if(tree == nullptr)
    {
        tree_status_errors(TREE_IS_NULL);
        return nullptr;
    }

    tree->root = nullptr;
    tree->elem_ctor = elem_ctor;
    tree->elem_dtor = elem_dtor;
    tree->write_elem = write_elem;
    tree->size = 0;

    return tree;
}

void tree_dtor(Tree* tree)
{
    assert(tree != NULL);

    tree_nodes_dtor(tree, tree->root);
    free(tree);
}

void tree_link_node(TreeNode* nodeParent, TreeNode* nodeSon)
{
    assert(nodeParent != nullptr);
    assert(nodeSon != nullptr);

    if(nodeParent->leftNode != nullptr && nodeParent->rightNode != nullptr)
        return;

    if(nodeParent->leftNode == nullptr)
        nodeParent->leftNode = nodeSon;
    else
        nodeParent->rightNode = nodeSon;
    
    nodeSon->parentNode = nodeParent;
}

TreeNode* tree_node_insert(Tree* tree, TreeNode* nodeParent, void* elem)
{
    assert(tree != nullptr);
    assert(elem != nullptr);

    if(tree->root == nullptr)
    {
        tree->root = tree_node_new(tree, elem);
        return tree->root;
    }

    TreeNode* nodeSon = tree_node_new(tree, elem);
    tree_link_node(nodeParent, nodeSon);

    return nodeSon;
}

void tree_node_delete(Tree* tree, TreeNode* node)
{
    assert(tree != nullptr);

    if(node == nullptr)
        return;

    if(node->parentNode != nullptr)
    {
        if(node->parentNode->leftNode == node)
            node->parentNode->leftNode = nullptr;
        else if(node->parentNode->rightNode == node)
            node->parentNode->rightNode = nullptr;
        else 
            assert(!"Error");
    }

    tree->elem_dtor(node->elem);
    free(node);

    tree->size--;
}

void tree_nodes_delete(Tree* tree, TreeNode* node)
{
    assert(tree != nullptr);

    if(node == nullptr)
        return;

    tree_nodes_delete(tree, node->leftNode);
    tree_nodes_delete(tree, node->rightNode);
    tree_node_delete(tree, node);

    tree->size--;
}