DEF_OP(OP_ADD,  "+",    1, ARG1 + ARG2,    DER_ADD())
DEF_OP(OP_SUB,  "-",    1, ARG1 - ARG2,    DER_SUB())
DEF_OP(OP_DIV,  "/",    1, ARG1 / ARG2,    DER_DIV())
DEF_OP(OP_MUL,  "*",    1, ARG1 * ARG2,    DER_MUL())
DEF_OP(OP_POW,  "^",    1, pow(ARG1,ARG2), DER_POW())
DEF_OP(OP_SIN,  "sin",  0, sin(ARG1),      DER_SIN())
DEF_OP(OP_COS,  "cos",  0, cos(ARG1),      DER_COS())
DEF_OP(OP_LN,   "ln",   0, log(ARG1),      DER_LN())
DEF_OP(OP_EXP,  "exp",  0, exp(ARG1),      DER_EXP())
DEF_OP(OP_SQRT, "sqrt", 0, sqrt(ARG1),     DER_SQRT())