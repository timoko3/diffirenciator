#ifndef DSL_H
#define DSL_H

#define _DIF(node, variableToDiff) differentiateNode(node, variableToDiff)
#define _L         node->left
#define _R         node->right
#define _C(node)   copyNode(node)

#define _N(value)  createNewNodeNumber    (value, NULL, NULL)
#define _V(name)   createNewNodeVariable  (name,  NULL, NULL)
#define _FACTOR(value) createNewNodeNumber(value, NULL, NULL)

#define _ADD(node1, node2) createNewNodeOperator("+", node1, node2)
#define _SUB(node1, node2) createNewNodeOperator("-", node1, node2)
#define _MUL(node1, node2) createNewNodeOperator("*", node1, node2)
#define _DIV(node1, node2) createNewNodeOperator("/", node1, node2)
#define _POW(node1, node2) createNewNodeOperator("^", node1, node2)

#define _COS(node1, node2) createNewNodeOperator("cos",   node1, node2)
#define _SIN(node1, node2) createNewNodeOperator("sin",   node1, node2)
#define _LN(node1, node2)  createNewNodeOperator("ln",    node1, node2)
#define _SH(node1, node2)  createNewNodeOperator("sinh",  node1, node2)
#define _CH(node1, node2)  createNewNodeOperator("cosh",  node1, node2)

#define _SQRT(node1, node2)  createNewNodeOperator("sqrt",  node1, node2)

#define _SCALE_GRAPH(node1, node2) createNewNodeOperator(":",  node1, node2)

#endif /* DSL_H */