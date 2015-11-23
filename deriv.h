// define enums for components of a mathematical expression
enum Node_type {NUMBER, VARIABLE, BINARY_OP, UNARY_OP};
enum Binary_op {BIN_OP_INVALID, BIN_OP_PLUS, BIN_OP_MINUS, BIN_OP_TIMES, 
                BIN_OP_DIVIDE, BIN_OP_EXP};
enum Unary_op {UN_OP_INVALID, UN_OP_NEG, UN_OP_EXP, UN_OP_LOG, UN_OP_SIN, UN_OP_COS};
enum Op {INVALID, PLUS, MINUS, TIMES, DIVIDE, BEXP, NEG, UEXP, LOG, SIN, COS, OPAR, 
         CPAR};

// define node 
// (which will go in expression tree to represent mathematical expression)
struct Node {
    enum Node_type node_type;
    int number;
    char variable_name;
    enum Binary_op binary_op;
    enum Unary_op unary_op;
    struct Node * left;
    struct Node * right;
    struct Node * only;
};

typedef struct Node * NodePtr;

// definition of a node stack
#define MAX_DEPTH 30
struct Stack {
    unsigned int depth;
    NodePtr elements[MAX_DEPTH];
};

// definition of operation (and parentheses) stack
struct Op_Stack {
    unsigned int depth;
    enum Op elements[MAX_DEPTH];
}; 

// print a node
void print(NodePtr node);

// create node for number
NodePtr num(int number);

// create node for binary operation
NodePtr bin_op(NodePtr left, NodePtr right, enum Binary_op binary_op); 

// create node for unary operation
NodePtr un_op(NodePtr only, enum Unary_op unary_op);

// create node for variable
NodePtr var(char variable_name); 

// free node from memory
void free_node(NodePtr node);

// copy node
NodePtr copy(NodePtr node); 

// differentiate node with respect to a variable
NodePtr diff(NodePtr node, char variable); 

// push node onto node stack
void push(struct Stack * stack, NodePtr node);

// pop node out of node stack
NodePtr pop(struct Stack * stack);

// push operation onto op stack
void push_op(struct Op_Stack * op_stack, enum Op op);

// pop operation out of op stack
enum Op pop_op(struct Op_Stack * op_stack);

// print the node stack
void print_stack(struct Stack * stack);

// print the op stack
void print_op_stack(struct Op_Stack * op_stack);

// converts enum Op to enum Binary_op
enum Binary_op To_Binary_op(enum Op op);

// converts enum Op to enum Unary_op
enum Unary_op To_Unary_op(enum Op op);

// pushes/pops to/from the node stack and op stack
// to be used when parsing mathematical expressions from command line
// (implementing Dijsktra's shunting-yard algorithm)
void handle_token(char * start, int len, struct Stack * stack, struct Op_Stack * op_stack, int * open_paren);

// parses input string (math expression) from command line
//  and returns node
NodePtr parse(char * str);

// evaluates node at a particular value of a variable
double eval(NodePtr node, char var, double val); 

// compares two nodes to see if they are equal
int compare(NodePtr node1, NodePtr node2, char var); 

// tests to see whether computed derivative of input node matches its known derivative
int testdiff(NodePtr node, NodePtr known_deriv, char var);
  
// tests functions
void test();
