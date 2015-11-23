#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include "deriv.h"

void print(NodePtr node) {
    switch(node->node_type) {
        case NUMBER:
            printf("%d", node->number);
            break;
        case VARIABLE:
            printf("%c", node->variable_name);
            break;
        case BINARY_OP:
            printf("(");
            print(node->left);
            printf(")");
            switch(node->binary_op) {
                case BIN_OP_PLUS: printf("+"); break;
                case BIN_OP_MINUS: printf("-"); break;
                case BIN_OP_TIMES: printf("*"); break;
                case BIN_OP_DIVIDE: printf("/"); break;
                case BIN_OP_EXP: printf("^"); break;
                default: break;
            }
            printf("(");
            print(node->right);
            printf(")");
            break;
        case UNARY_OP:
            switch(node->unary_op) {
                case UN_OP_NEG: printf("-"); break;
                case UN_OP_EXP: printf("exp"); break;
                case UN_OP_LOG: printf("log"); break;
                case UN_OP_SIN: printf("sin"); break;
                case UN_OP_COS: printf("cos"); break;
                default: break;
            }
            printf("(");
            print(node->only);
            printf(")");
            break;
        default: break;
    }
}

NodePtr num(int number) {
    NodePtr node = malloc(sizeof(struct Node));
    node->node_type = NUMBER;
    node->number = number;
    return node;
}   

NodePtr bin_op(NodePtr left, NodePtr right, enum Binary_op binary_op) {
    NodePtr node = malloc(sizeof(struct Node));
    node->node_type = BINARY_OP;
    node->binary_op = binary_op;
    node->left = left;
    node->right = right;
    return node;
}

NodePtr un_op(NodePtr only, enum Unary_op unary_op) {
    NodePtr node = malloc(sizeof(struct Node));
    node->node_type = UNARY_OP;
    node->unary_op = unary_op;
    node->only = only;
    return node;
}

NodePtr var(char variable_name) {
    NodePtr node = malloc(sizeof(struct Node));
    node->node_type = VARIABLE;
    node->variable_name = variable_name;
    return node;
}

void free_node(NodePtr node) {
    switch(node->node_type) {
        case BINARY_OP:
            free_node(node->left);
            free_node(node->right);
            break;
        case UNARY_OP:
            free_node(node->only);
            break;
        default: break;
    }
    free(node);
}

NodePtr copy(NodePtr node) {
    NodePtr node_copy = malloc(sizeof(struct Node));
    node_copy->node_type = node->node_type;
    switch(node->node_type) {
        case NUMBER:
            node_copy->number = node->number;
            break;
        case VARIABLE:
            node_copy->variable_name = node->variable_name;
            break;
        case BINARY_OP:
            node_copy->binary_op = node->binary_op;
            node_copy->left = copy(node->left);
            node_copy->right = copy(node->right);
            break;
        case UNARY_OP:
            node_copy->unary_op = node->unary_op;
            node_copy->only = copy(node->only);
            break;
        default: break;
    }
    return node_copy;
}

NodePtr diff(NodePtr node, char variable) {
    NodePtr diff_node = NULL; 
    switch(node->node_type) {
        case NUMBER:
            diff_node = num(0);
            break;
        case VARIABLE:
            if (node->variable_name == variable) {
                diff_node = num(1);
            }
            else {
                diff_node = num(0);
            }
            break;
        case BINARY_OP:
            switch(node->binary_op) {
                case BIN_OP_PLUS:
                    diff_node = bin_op(diff(node->left, variable), 
                                       diff(node->right, variable), 
                                       BIN_OP_PLUS);
                    break;
                case BIN_OP_MINUS:
                    diff_node = bin_op(diff(node->left, variable), 
                                       diff(node->right, variable), 
                                       BIN_OP_MINUS);
                    break;
                case BIN_OP_TIMES:
                    diff_node = bin_op(bin_op(diff(node->left, variable), 
                                              copy(node->right), BIN_OP_TIMES), 
                                       bin_op(copy(node->left), 
                                              diff(node->right, variable), 
                                              BIN_OP_TIMES), 
                                       BIN_OP_PLUS);
                    break;
                case BIN_OP_DIVIDE:
                    diff_node = bin_op(bin_op(bin_op(diff(node->left, variable),
                                                     copy(node->right), 
                                                     BIN_OP_TIMES), 
                                              bin_op(copy(node->left), 
                                                     diff(node->right, variable), 
                                                     BIN_OP_TIMES), 
                                              BIN_OP_MINUS), 
                                       bin_op(copy(node->right), 
                                              copy(node->right), BIN_OP_TIMES), 
                                       BIN_OP_DIVIDE);
                    break;
                case BIN_OP_EXP:
                    if ((node->right)->node_type == NUMBER) {  // easy case: power rule
                        diff_node = bin_op(bin_op(num((node->right)->number), 
                                                  bin_op(copy(node->left), 
                                                         num((node->right)->number - 1),
                                                         BIN_OP_EXP), 
                                                  BIN_OP_TIMES), 
                                           diff(node->left, variable), 
                                           BIN_OP_TIMES); 
                    }
                    else {  // general case
                        NodePtr rewritten_node = un_op(bin_op(copy(node->right),
                                                                    un_op(copy(node->left), 
                                                                          UN_OP_LOG), 
                                                                    BIN_OP_TIMES), 
                                                             UN_OP_EXP); 
                        diff_node = diff(rewritten_node, variable);
                        free_node(rewritten_node);
                    }
                    break;
                default: break;
            }
            break;
        case UNARY_OP:
            switch(node->unary_op) {
                case UN_OP_NEG:
                    diff_node = un_op(diff(node->only, variable), UN_OP_NEG);
                    break;
                case UN_OP_EXP:
                    diff_node = bin_op(diff(node->only, variable), 
                                       un_op(copy(node->only), UN_OP_EXP), 
                                       BIN_OP_TIMES);
                    break;
                case UN_OP_LOG: 
                    diff_node = bin_op(diff(node->only, variable), 
                                       copy(node->only), BIN_OP_DIVIDE);
                    break;
                case UN_OP_SIN:
                    diff_node = bin_op(un_op(copy(node->only), UN_OP_COS), 
                                       diff(node->only, variable), BIN_OP_TIMES);
                    break;
                case UN_OP_COS:
                    diff_node = bin_op(un_op(un_op(copy(node->only), UN_OP_SIN),
                                             UN_OP_NEG), 
                                       diff(node->only, variable), BIN_OP_TIMES);
                    break;
                default: break;
            }
            break;
        default: break;
    }
    return diff_node;
}

void push(struct Stack * stack, NodePtr node) {
    stack->elements[stack->depth] = node;
    stack->depth++;
}

NodePtr pop(struct Stack * stack) {
    stack->depth--;
    return stack->elements[stack->depth];
}

void push_op(struct Op_Stack * op_stack, enum Op op) {
    op_stack->elements[op_stack->depth] = op;
    op_stack->depth++;
}

enum Op pop_op(struct Op_Stack * op_stack) {
    op_stack->depth--;
    return op_stack->elements[op_stack->depth];
}

void print_stack(struct Stack * stack) {
    printf("bottom of node stack\n");
    unsigned int i;
    for (i = 0 ; i < stack->depth; i++) {
        printf("    element at %u is:  ", i);
        print(stack->elements[i]);
        printf("\n");
    }
    printf("top of node stack\n\n");
}

void print_op_stack(struct Op_Stack * op_stack) {
    printf("bottom of op stack\n");
    unsigned int i;
    for (i = 0; i < op_stack->depth; i++) {
        printf("    element at %u is:  ", i);
        switch(op_stack->elements[i]) {
            case PLUS:
                printf("+");
                break;
            case MINUS:
                printf("-");
                break;
            case TIMES:
                printf("*");
                break;
            case DIVIDE:
                printf("/");
                break;
            case BEXP:
                printf("^");
                break;
            case NEG:
                printf("-");
                break;
            case UEXP:
                printf("exp");
                break;
            case LOG:
                printf("log");
                break;
            case SIN:
                printf("sin");
                break;
            case COS:
                printf("cos");
                break;
            case OPAR:
                printf("(");
                break;
            case CPAR:
                printf(")");
                break;
            default: break;
        }
        printf("\n");
    }
    printf("top of op stack\n\n");
}

enum Binary_op To_Binary_op(enum Op op) {
    enum Binary_op conv_op = BIN_OP_INVALID;
    switch(op) {
        case PLUS:
            conv_op = BIN_OP_PLUS;
            break;
        case MINUS:
            conv_op = BIN_OP_MINUS;
            break;
        case TIMES:
            conv_op = BIN_OP_TIMES;
            break;
        case DIVIDE:
            conv_op = BIN_OP_DIVIDE;
            break;
        case BEXP:
            conv_op = BIN_OP_EXP;
            break;  
        default: break;
    } 
    return conv_op;
}

enum Unary_op To_Unary_op(enum Op op) {
    enum Unary_op conv_op = UN_OP_INVALID;
    switch(op) {
        case NEG:
            conv_op = UN_OP_NEG;
            break;
        case UEXP:
            conv_op = UN_OP_EXP;
            break;
        case LOG:
            conv_op = UN_OP_LOG;
            break;
        case SIN:
            conv_op = UN_OP_SIN;
            break;
        case COS:
            conv_op = UN_OP_COS;
            break;
        default: break;
    }
    return conv_op;
}

void handle_token(char * start, int len, struct Stack * stack, struct Op_Stack * op_stack, int * open_paren) {
    // this gets set to 1 if token is an open parenthesis
    int new_open_paren = 0;

    // operations
    if (start[0] == '+') {
        // printf("found special PLUS symbol\n");
        push_op(op_stack, PLUS); 
    }
    else if (start[0] == '-') { // distinguish between minus and neg
        if (* open_paren == 1) {
            // printf("found special NEG symbol\n");
            push_op(op_stack, NEG);
        }
        else {
            // printf("found special MINUS symbol\n");
            push_op(op_stack, MINUS);
        }
    }
    else if (start[0] == '*') {
        // printf("found special TIMES symbol\n");
        push_op(op_stack, TIMES);
    }
    else if (start[0] == '/') {
        // printf("found special DIVIDE symbol\n");
        push_op(op_stack, DIVIDE); 
    }
    else if (start[0] == '^') {
        // printf("found special BEXP symbol\n");
        push_op(op_stack, BEXP); 
    }
    else if (strncmp(start, "exp", 3) == 0) {
        // printf("found special UEXP symbol\n");
        push_op(op_stack, UEXP);
    }
    else if (strncmp(start, "log", 3) == 0) {
        // printf("found special LOG symbol\n");
        push_op(op_stack, LOG);
    }
    else if (strncmp(start, "sin", 3) == 0) {
        // printf("found special SIN symbol\n");
        push_op(op_stack, SIN);
    }
    else if (strncmp(start, "cos", 3) == 0) {
        // printf("found special COS symbol\n");
        push_op(op_stack, COS);
    }

    // parentheses
    else if (start[0] == '(') {
        // printf("found special OPAR symbol\n");
        push_op(op_stack, OPAR);
        new_open_paren = 1; 
    }
    else if (start[0] == ')') {
        // printf("found special CPAR symbol\n");
        // pop all the elements off op stack until reach an open parenthesis
        // and use them to build a new node to push onto the node stack
        while (1) {
            enum Op op = pop_op(op_stack);
            if ((op == NEG) || (op == UEXP) || (op == LOG) || (op == SIN) 
                  || (op == COS)) {
                 // create unary op struct node and push it to node stack
                 //NodePtr element = un_op(pop(& stack), op);
                 //push(& stack, element);
                 NodePtr only = pop(stack);
                 NodePtr new_node = un_op(only, To_Unary_op(op));
                 push(stack, new_node);
             }
             else if ((op == PLUS) || (op == MINUS) || (op == TIMES) 
                      || (op == DIVIDE) || (op == BEXP)) {
                 // create binary op struct node and push it to node stack
                 NodePtr right = pop(stack);
                 NodePtr left = pop(stack);
                 NodePtr new_node = bin_op(left, right, To_Binary_op(op));
                 push(stack, new_node);
             }
             else if (op == OPAR) {
                 break;
             } 
        }
    } 

    // one letter variables
    else if (len == 1 && ((start[0] >= 'a' && start[0] <= 'z') || 
                          (start[0] >= 'A' && start[0] <= 'Z'))) {
        // printf("found a variable named: %c\n", start[0]);
        // create the variable node and push it to node stack
        push(stack, var(start[0]));
    }

    // integers 
    else if (len >= 1) {
        int i = 0;
        while (i < len && (start[i] >= '0' && start[i] <= '9')) {
            i++;
        }
        if (i == len) {
            // convert string to int
            char copy[32];
            memcpy(copy, start, len);
            copy[len] = 0;
            int value = atoi(copy);
            // printf("found an integer valued: %d\n", value); 
            // create the num node and push it to node stack
            push(stack, num(value));
        }
        else {
            printf("bad input\n");
        }
    }
    * open_paren = new_open_paren;
    // print_stack(stack);
    // print_op_stack(op_stack);
}

NodePtr parse(char * str) {
    // index of start of most recent token
    int token = 0;
    // index of current location
    int curr = 0;

    // declare node stack
    struct Stack stack;
    stack.depth = 0;
    // declare op stack
    struct Op_Stack op_stack;
    op_stack.depth = 0;

    // declare a variable to keep track of when a token is an open parenthesis
    int open_paren = 0;

    while(1) {
        if (str[curr] == 0) {
            if (token < curr) {  // finishing a token
                // print str[token, curr)
                handle_token(& (str[token]), curr - token, & stack, & op_stack, & open_paren);
            }
            break;
        }
        else if (str[curr] == ' ') {  // finished a token
            // print str[token, curr)
            handle_token(& (str[token]), curr - token, & stack, & op_stack, & open_paren);
            // increment curr and token
            curr++;
            token = curr;
        }
        else {  // move on
            // increment curr
            curr++;
        }
    }

    // return the result from node stack
    if (stack.depth == 1) {
        return stack.elements[0];
    }
    else {
        return NULL;
    }
}

double eval(NodePtr node, char var, double val) {
    switch(node->node_type) {
        case NUMBER:
            return (double) node->number;
        case VARIABLE:
            return val;
        case BINARY_OP:
            {
            double eval_left = eval(node->left, var, val);
            double eval_right = eval(node->right, var, val);
            switch(node->binary_op) {
                case BIN_OP_PLUS: 
                    return (eval_left + eval_right); 
                case BIN_OP_MINUS: 
                    return (eval_left - eval_right);
                case BIN_OP_TIMES:  
                    return (eval_left * eval_right);
                case BIN_OP_DIVIDE:  
                    if (eval_right !=0) {
                        return (eval_left / eval_right);
                    }
                    else {
                        printf("Division by zero, cannot proceed\n");
                        break;
                    }
                case BIN_OP_EXP: 
                    return pow(eval_left, eval_right);
                default: break;
            }
            }
        case UNARY_OP:
            {
            double eval_only = eval(node->only, var, val);
            switch(node->unary_op) {
                case UN_OP_NEG: 
                    return -eval_only;
                case UN_OP_EXP: 
                    return exp(eval_only);
                case UN_OP_LOG: 
                    if (eval_only > 0) {
                        return log(eval_only);
                    }
                    else {
                        printf("Value not in domain of log, cannot proceed\n");
                        break;
                    }
                case UN_OP_SIN: 
                    return sin(eval_only);
                case UN_OP_COS: 
                    return cos(eval_only);
                default: break;
            }
            }
    }
    return 0;
}

int compare(NodePtr node1, NodePtr node2, char var) {
    // srand(time(NULL));
    unsigned int size = 5;  // number of inputs
    double input[size];
    double a = 10;  // to get boundaries of range for inputs
    double tolerance = 1e-10;  // equality up to this much of a difference
    int i;
    for (i = 0; i < size; i++) {
        input[i] = (((double) rand() / (double) (RAND_MAX)) * 2 * a) - a;
        // printf("Random number %d is %f\n", i, input[i]);
    }
    double eval_node1, eval_node2;
    for (i = 0; i < size; i++) {
        // evaluations
        eval_node1 = eval(node1, var, input[i]);
        eval_node2 = eval(node2, var, input[i]);
       
        /* 
        // print to test
        printf("The nodes ");
        print(node1);
        printf(" and ");
        print(node2);
        printf(" evaluate to ");
        printf("%f and %f respectively\n", eval_node1, eval_node2);
        */

        // comparison
        if (fabs(eval_node1 - eval_node2) > tolerance) {
            // in this case, the expressions do not evaluate to the same number
           return 0;
        }
    } 
    return 1;
}

int testdiff(NodePtr node, NodePtr known_deriv, char var) {
    NodePtr computed_deriv = diff(node, var);
    if (compare(computed_deriv, known_deriv, var)) {
        return 1;
    }
    return 0;
}

void test() {
    // some sample nodes
    NodePtr node_const = num(5); 
    print(node_const);
    printf("\n");
    NodePtr node = bin_op(num(5), var('x'), BIN_OP_MINUS);
    print(node);
    printf("\n");
    NodePtr node1 = un_op(bin_op(var('x'), num(5), BIN_OP_MINUS), UN_OP_NEG);
    print(node);
    printf("\n");
    NodePtr test_node = bin_op(un_op(bin_op(bin_op(un_op(num(2), UN_OP_NEG),
                                                   var('x'), BIN_OP_TIMES),
                                            num(1), BIN_OP_PLUS), 
                                     UN_OP_EXP), 
                               bin_op(num(3), bin_op(var('x'), num(2), 
                                                     BIN_OP_EXP), 
                                      BIN_OP_TIMES),
                               BIN_OP_MINUS);
    print(test_node);
    printf("\n");
    NodePtr test_cos0 = un_op(bin_op(num(10), var('x'), BIN_OP_TIMES), UN_OP_COS);
    print(test_cos0);
    printf("\n");
    NodePtr test_cos = un_op(bin_op(bin_op(num(10), var('x'), 
                                           BIN_OP_TIMES), 
                                    var('y'), 
                                    BIN_OP_PLUS), 
                             UN_OP_COS);
    print(test_cos);
    printf("\n");
    NodePtr test_sin = un_op(bin_op(un_op(bin_op(var('x'), num(3), 
                                                 BIN_OP_EXP), 
                                          UN_OP_LOG), 
                                    bin_op(bin_op(var('x'), var('y'), 
                                                  BIN_OP_TIMES), 
                                           bin_op(num(5), var('x'), 
                                                  BIN_OP_TIMES), 
                                           BIN_OP_PLUS), 
                                    BIN_OP_PLUS), 
                             UN_OP_SIN);
    print(test_sin);
    printf("\n");

    // test the copy function for nodes
    NodePtr node_const_copy = copy(node_const);
    print(node_const_copy);
    printf("\n");
    NodePtr node_copy = copy(node);
    print(node_copy);
    printf("\n");
    NodePtr node_copy2 = copy(test_node);
    print(node_copy2);
    printf("\n");
    NodePtr node_copy3 = copy(test_cos);
    print(node_copy3);
    printf("\n");
    NodePtr node_copy4 = copy(test_sin);
    print(node_copy4);
    printf("\n");

    // test the differentation function
    NodePtr diff_node_const = diff(node_const, 'x');
    print(diff_node_const);
    printf("\n");
    NodePtr diff_node = diff(node, 'x');
    print(diff_node);
    printf("\n");
    NodePtr diff_node2 = diff(test_node, 'x');
    print(diff_node2);
    printf("\n");
    NodePtr diff_node3 = diff(test_cos, 'x');
    print(diff_node3);
    printf("\n");
    NodePtr diff_node4 = diff(test_sin, 'x');
    print(diff_node4);
    printf("\n");
    
    // test the node stack and pop and push and print function for node stack
    struct Stack s;
    s.depth = 0;
    push(& s, node);
    push(& s, test_node);
    push(& s, test_cos);
    print_stack(& s);
    printf("first pop:   ");
    print(pop(& s));
    printf("\n");
    printf("second pop:   ");
    print(pop(& s));
    printf("\n");    
    push(& s, test_sin);
    printf("another pop:   ");
    print(pop(& s));
    printf("\n");

    // test the op stack and pop and push for op stack
    struct Op_Stack os;
    os.depth = 0;
    enum Op op1 = PLUS;
    enum Op op2 = OPAR;
    push_op(& os, op1);
    printf("pushed to op stack: %d\n", op1);
    push_op(& os, op2);
    printf("pushed to op stack: %d\n", op2);
    push_op(& os, CPAR);
    printf("pushed to op stack: %d\n", CPAR);
    printf("Here is first pop from op stack: %d\n", pop_op(& os)); 
    printf("Here is second pop from op stack: %d\n", pop_op(& os));

    // test the evaluation function
    double val = 3.14;
    char var = 'x';
    double eval_node_const = eval(node_const, var, val);
    printf("The expression ");
    print(node_const);
    printf(" evaluated at %c = %f is %f\n", var, val, eval_node_const); 
    double eval_node = eval(node, var, val);
    printf("The expression ");
    print(node);
    printf(" evaluated at %c = %f is %f\n", var, val, eval_node);
    double eval_test_node = eval(test_node, var, val);
    printf("The expression ");
    print(test_node);
    printf(" evaluated at %c = %f is %f\n", var, val, eval_test_node);
    double eval_test_cos0 = eval(test_cos0, var, val);
    printf("The expression ");
    print(test_cos0);
    printf(" evaluated at %c = %f is %f\n", var, val, eval_test_cos0);
    double eval_test_cos = eval(test_cos, var, val);
    printf("The expression ");
    print(test_cos);
    printf(" evaluated at %c = %f is %f\n", var, val, eval_test_cos);
    
    // test comparison function
    int comp = compare(node, node1, var);
    printf("The nodes ");
    print(node);
    printf(" and ");
    print(node1);
    if (comp) {
        printf(" are equal\n");
    }
    else {
        printf(" are not equal\n");
    }
    comp = compare(test_node, test_cos0, var);
    printf("The nodes ");
    print(test_node);
    printf(" and ");
    print(test_cos0);
    if (comp) {
        printf(" are equal\n");
    }
    else {
        printf(" are not equal\n");
    }
  
    // free all the nodes
    free_node(node_const);
    free_node(node);
    free_node(node1);
    free_node(test_node);
    free_node(test_cos0);
    free_node(test_cos);
    free_node(test_sin);
    free_node(node_const_copy);
    free_node(node_copy);
    free_node(node_copy2);
    free_node(node_copy3);
    free_node(node_copy4);
    free_node(diff_node_const);
    free_node(diff_node);
    free_node(diff_node2);
    free_node(diff_node3);
    free_node(diff_node4);
}
    
int main(int argc, char** argv){
    // differenatiation command
    if (argc == 4 && strcmp(argv[1], "-diff") == 0) {
        // use parse function to convert second argument to node
        NodePtr expression = parse(argv[2]);
        printf("Your input is:\n");
        print(expression);
        printf("\n");

        // convert third argument to char
        char variable = 0;
        if (strlen(argv[3]) == 1) {
            variable = argv[3][0];
        }

        // differentiate 
        printf("Its derivative with respect to %c is:\n", variable);
        NodePtr derivative = diff(expression, variable);
        print(derivative);
        printf("\n\n");

        free_node(expression);
        free_node(derivative);
    }
   
    // evaluation command
    if (argc == 5 && strcmp(argv[1], "-eval") == 0) {
        // use parse function to convert second argument to node
        NodePtr expression = parse(argv[2]);
  
        // convert third argument to char
        char variable = 0;
        if (strlen(argv[3]) == 1) {
            variable = argv[3][0];
        }

        // convert fourth argument to double
        double value = 0;
        value = atof(argv[4]);
         
        // evaluate
        double evaluation = eval(expression, variable, value);
        printf("The expression ");
        print(expression);
        printf(" evaluated at %c = %f is equal to %f\n\n", variable, value, evaluation);

        free_node(expression);
    }

    // test derivative command
    if (argc == 5 && strcmp(argv[1], "-test") == 0) {
        // use parse function to convert second and third arguments to nodes
        NodePtr expression = parse(argv[2]);
        NodePtr known_derivative = parse(argv[3]);

        // convert fourth argument to char
        char variable = 0;
        if (strlen(argv[4]) == 1) {
            variable = argv[4][0];
        }

        // computer derivative of second argument
        NodePtr computed_derivative = diff(expression, variable);

        printf("The derivative with respect to %c of ", variable);
        print(expression);
        printf(" is\n");
        print(computed_derivative);
          
        // compare computed derivative and known derivative
        int comp = compare(computed_derivative, known_derivative, variable);
        if (comp) {
           printf("\nwhich is the same as ");
        }
        else {
           printf("\nwhich is not the same as ");
        }
        print(known_derivative); 
        printf("\n");

        free_node(expression);
        free_node(known_derivative);
        free_node(computed_derivative);

        return !comp; 
    } 
   
    // none of the above
    else {
        printf("Command invalid. It should look like one of: \n");
        printf("%s -diff <expression> <variable>\n", argv[0]);
        printf("%s -eval <expression> <variable> <value>\n", argv[0]);
        printf("%s -test <expression1> <expression2> <variable>\n", argv[0]);
    } 
    // test();

    return 0;
}
