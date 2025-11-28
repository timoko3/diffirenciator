#ifndef DSL_H
#define DSL_H

#define _DIF(node) differentiateNode(node)
#define _L         node->left
#define _R         node->right
#define _C(node)   copyNode(node)

#define _V(name)   createNewNodeVariable(name, NULL, NULL)

#define _ADD(node1, node2) createNewNodeOperator("+", node1, node2)
#define _MUL(node1, node2) createNewNodeOperator("*", node1, node2)

#define _COS(node1, node2) createNewNodeOperator("cos", node1, node2)

#endif /* DSL_H */