#include <assert.h>
#include <string.h>

#include "Calculate_derivative.h"
#include "Parsing.h"
#include "Differentiator.h"
#include "Node.h"

TreeNode* derivative_variable()
{
    Data elem;
    elem.value = 1;
    return create_node(NUM, elem, nullptr, nullptr);
}

TreeNode* derivative_const()
{
    Data elem;
    elem.value = 0;
    return create_node(NUM, elem, nullptr, nullptr);
}

TreeNode* derivative_sum(Tree* expression, TreeNode* expressionNode)
{
    assert(expressionNode != nullptr);

    return create_node(OPERATOR, ((NodeData*)(expressionNode->elem))->elem,
                            calculate_derivative(expression, expressionNode->leftNode),
                            calculate_derivative(expression, expressionNode->rightNode));
}

TreeNode* derivative_mul(Tree* expression, TreeNode* expressionNode)
{
    assert(expressionNode != nullptr);
    Data elem;

    TreeNode* node11 = copy_node(expressionNode->leftNode);
    TreeNode* node21 = copy_node(expressionNode->rightNode);

    elem.op = OP_MUL;
    TreeNode* node1 = create_node(OPERATOR, elem, node11,
                                calculate_derivative(expression, expressionNode->rightNode));

    elem.op = OP_MUL;
    TreeNode* node2 = create_node(OPERATOR, elem, node21,
                                calculate_derivative(expression, expressionNode->leftNode));

    elem.op = OP_ADD;
    TreeNode* node = create_node(OPERATOR, elem, node1, node2);

    return node;
}

TreeNode* derivative_div(Tree* expression, TreeNode* expressionNode)
{
    assert(expressionNode != nullptr);
    Data elem;

    TreeNode* node121 = copy_node(expressionNode->leftNode);
    TreeNode* node111 = copy_node(expressionNode->rightNode);
    TreeNode* node21 = copy_node(expressionNode->rightNode);

    elem.value = 2;
    TreeNode* node22 = create_node(NUM, elem, nullptr, nullptr);

    elem.op = OP_POW;
    TreeNode* node2 = create_node(OPERATOR, elem, node21, node22);

    elem.op = OP_MUL;
    TreeNode* node11 = create_node(OPERATOR, elem, node111,
                                calculate_derivative(expression, expressionNode->leftNode));

    elem.op = OP_MUL;
    TreeNode* node12 = create_node(OPERATOR, elem, node121,
                                calculate_derivative(expression, expressionNode->rightNode));
    elem.op = OP_SUB;
    TreeNode* node1 = create_node(OPERATOR, elem, node11, node12);

    elem.op = OP_DIV;
    TreeNode* node = create_node(OPERATOR, elem, node1, node2);

    return node;
}

TreeNode* derivative_pow(Tree* expression, TreeNode* expressionNode)
{
    assert(expressionNode != nullptr);
    Data elem;

    TreeNode* node1121 = copy_node(expressionNode->rightNode);

    elem.value = 1;
    TreeNode* node1122 = create_node(NUM, elem, nullptr, nullptr);

    elem.op = OP_SUB;
    TreeNode* node112 = create_node(OPERATOR, elem, node1121, node1122);

    TreeNode* node111 = copy_node(expressionNode->leftNode);

    elem.op = OP_POW;
    TreeNode* node11 = create_node(OPERATOR, elem, node111, node112);

    TreeNode* node121 = copy_node(expressionNode->rightNode);

    elem.op = OP_MUL;
    TreeNode* node12 = create_node(OPERATOR, elem, node121,
                                calculate_derivative(expression, expressionNode->leftNode));

    elem.op = OP_MUL;
    TreeNode* node1 = create_node(OPERATOR, elem, node11, node12);

    TreeNode* node2212 = copy_node(expressionNode->leftNode);

    elem.op = OP_LN;
    TreeNode* node221 = create_node(OPERATOR, elem, node2212, nullptr);

    elem.op = OP_MUL;
    TreeNode* node22 = create_node(OPERATOR, elem, node221,
                                calculate_derivative(expression, expressionNode->rightNode));

    TreeNode* node211 = copy_node(expressionNode->leftNode);
    TreeNode* node212 = copy_node(expressionNode->rightNode);

    elem.op = OP_POW;
    TreeNode* node21 = create_node(OPERATOR, elem, node211, node212);

    elem.op = OP_MUL;
    TreeNode* node2 = create_node(OPERATOR, elem, node21, node22);

    elem.op = OP_ADD;
    TreeNode* node = create_node(OPERATOR, elem, node1, node2);

    return node;
}

TreeNode* derivative_ln(Tree* expression, TreeNode* expressionNode)
{
    assert(expressionNode != nullptr);
    Data elem;

    TreeNode* node2 = copy_node(expressionNode->leftNode);

    elem.op = OP_DIV;
    TreeNode* node = create_node(OPERATOR, elem,
                        calculate_derivative(expression, expressionNode->leftNode), node2);

    return node;
}

TreeNode* derivative_sqrt(Tree* expression, TreeNode* expressionNode)
{
    assert(expressionNode != nullptr);
    Data elem;

    ((NodeData*)(expressionNode->elem))->elem.op = OP_POW;

    elem.value = 0.5;
    tree_link_node(expressionNode, create_node(NUM, elem, nullptr, nullptr));

    return derivative_pow(expression, expressionNode);
}

TreeNode* derivative_exp(Tree* expression, TreeNode* expressionNode)
{
    assert(expressionNode != nullptr);
    Data elem;

    TreeNode* node1 = copy_node(expressionNode);

    elem.op = OP_MUL;
    TreeNode* node = create_node(OPERATOR, elem, node1,
                                calculate_derivative(expression, expressionNode->leftNode));

    return node;
}

TreeNode* derivative_sin(Tree* expression, TreeNode* expressionNode)
{
    assert(expressionNode != nullptr);
    Data elem;

    TreeNode* node11 = copy_node(expressionNode->leftNode);

    elem.op = OP_COS;
    TreeNode* node1 = create_node(OPERATOR, elem, node11, nullptr);

    elem.op = OP_MUL;
    TreeNode* node = create_node(OPERATOR, elem, node1,
                                calculate_derivative(expression, expressionNode->leftNode));

    return node;
}

TreeNode* derivative_cos(Tree* expression, TreeNode* expressionNode)
{
    assert(expressionNode != nullptr);
    Data elem;

    TreeNode* node121 = copy_node(expressionNode->leftNode);

    elem.op = OP_SIN;
    TreeNode* node12 = create_node(OPERATOR, elem, node121, nullptr);

    elem.value = -1;
    TreeNode* node11 = create_node(NUM, elem, nullptr, nullptr);

    elem.op = OP_MUL;
    TreeNode* node1 = create_node(OPERATOR, elem, node11, node12);

    elem.op = OP_MUL;
    TreeNode* node = create_node(OPERATOR, elem, node1,
                                calculate_derivative(expression, expressionNode->leftNode));

    return node;
}

TreeNode* calculate_derivative(Tree* expression, TreeNode* expressionNode)
{
    assert(expressionNode != nullptr);

    const char* VARIABLE = "x";

    if(((NodeData*)(expressionNode->elem))->type == OPERATOR)
    {
        switch(((NodeData*)(expressionNode->elem))->elem.op)
        {
            #define ARG expression, expressionNode
            #define DER_ADD() derivative_sum(ARG)
            #define DER_SUB() derivative_sum(ARG)
            #define DER_MUL() derivative_mul(ARG)
            #define DER_DIV() derivative_div(ARG)
            #define DER_POW() derivative_pow(ARG)
            #define DER_LN() derivative_ln(ARG)
            #define DER_SQRT() derivative_sqrt(ARG)
            #define DER_EXP() derivative_exp(ARG)
            #define DER_SIN() derivative_sin(ARG)
            #define DER_COS() derivative_cos(ARG)

            #define DEF_OP(name, icon, binary_op, code, der)    \
                case name:                                      \
                    return der;
            default:
                tree_dtor(expression);
                assert(!"Unknown operator");

            #include "Operators.h"

            #undef DEF_OP
            #undef DER_ADD
            #undef DER_SUB
            #undef DER_MUL
            #undef DER_DIV
            #undef DER_POW
            #undef DER_SQRT
            #undef DER_EXP
            #undef DER_LN
            #undef DER_SIN
            #undef DER_COS
        }
    }
    else if(((NodeData*)(expressionNode->elem))->type == IDENTIFIER &&
                !strcmp(((NodeData*)(expressionNode->elem))->elem.id, VARIABLE))
        return derivative_variable();
    else if(((NodeData*)(expressionNode->elem))->type == NUM ||
                ((NodeData*)(expressionNode->elem))->type == IDENTIFIER)
        return derivative_const();
    else
    {
        tree_dtor(expression);
        assert(!"Unknown type");
    }

    return nullptr;
}
