#ifndef DSL_H
#define DSL_H

#define _DIF(node) differentiateNode(node)
#define _L         toDifferentiate->left
#define _R         toDifferentiate->right
#define _C(node)   copyNode(node)

#define _ADD(node1, node2) createNewNodeOperator("+", node1, node2)
#define _MUL(node1, node2) createNewNodeOperator("*", node1, node2)

#endif /* DSL_H */