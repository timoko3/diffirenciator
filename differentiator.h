#include "generalDifferentiator.h"

typedef treeNode_t* curAnchorNode;

curAnchorNode differentiatorCtor(differentiator_t* differentiator);
curAnchorNode differentiatorDtor(differentiator_t* differentiator);

differentiator_t differentiate(differentiator_t* differentiator);

void differentiatorReadData(differentiator_t* differentiator);