#include "visitor.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include "scope.h"
#include "exception.h"
#include "token.h"
#include "debug.h"

/*(Standard GScript Library Function) Prints ARGV to console */
static AST *std_func_write(Visitor *visitor, AST **argv, int argc, unsigned line) {
  for (int i = 0; i < argc; i++) {
    AST *visited_ast = visitor_visit(visitor, argv[i]);
    switch (visited_ast->type) {
    case AST_STR:
      printf("%s", visited_ast->str_value);
      break;
    case AST_NUM:
      printf("%lf", visited_ast->num_value);
      break;
    case AST_BOOL:
      if(visited_ast->b_value==1) printf("true"); else printf("false"); break;
    default:
      _GSERR(line, "GS504 - Illegal argument to function 'write'");
    }
  }
  printf("\n");
  //AST* a = scope_get_function_definition(visitor->current_scope, "recur");
  //printf("%lf\n", a->func_def_args[0]->num_value);
  return ast_init(AST_NOOP);
}
/*(Standard GScript Library Function) Reads next string input from console and
 * returns AST node with it*/
static AST *std_func_read(Visitor *visitor, AST **argv, int argc, unsigned line) {
  if (argc > 1) {
    char d[100];
    sprintf(d, "GS502 - Function 'read' does not take %d arguments", argc);
    _GSERR(line, d );
   // exit(1);
    return ast_init(AST_NOOP);
  } else {
    AST *visited_ast = visitor_visit(visitor, argv[0]);
    switch (visited_ast->type) {
    case AST_STR: {
      printf("%s", visited_ast->str_value);
      char *result = malloc(sizeof(char *));
      fgets(result, 1000, stdin);
      if ((strlen(result) > 0) && (result[strlen(result) - 1] == '\n'))
        result[strlen(result) - 1] = '\0';
      AST *uinput_ast = ast_init(AST_STR);
      uinput_ast->str_value = result;
      return uinput_ast;
    }
      default: {
        _GSERR(line, "GS504 - Illegal argument to function 'read'");
        break;
      }
    }
  }
}
/*(Standard GScript Library Function) Converts argument string to uppercase and returns
* AST node with it*/
static AST *std_func_toupper(Visitor* visitor, AST **argv, int argc, unsigned line) {
  if((argv[0]->str_value == NULL && argv[0]->func_call_name == NULL && argv[0]->var_name == NULL && argv[0]->type != AST_NUM) || argv[0]->type == AST_NOOP) {
    _GSERR(line, "GS501 - No arguments provided where needed for function 'toupper'");
    return ast_init(AST_NOOP);
  }
  if (argc != 1) {
    char d[100];
    sprintf(d, "GS502 - Function 'toupper' does not take %d arguments", argc);
    _GSERR(line, d );
    return ast_init(AST_NOOP);
  } else {
    AST* visited_ast = visitor_visit(visitor, argv[0]);
    switch(visited_ast->type){
      case AST_STR: {
        AST *upper_ast = ast_init(AST_STR);
       // char result[strlen(visited_ast->str_value)];

        for(int i = 0; i < strlen(visited_ast->str_value); i++){
          if(isalpha(visited_ast->str_value[i])) {
            visited_ast->str_value[i] = toupper(visited_ast->str_value[i]);
          } else {
            visited_ast->str_value[i] = visited_ast->str_value[i];
          }
        }
        upper_ast->str_value = visited_ast->str_value;
        return upper_ast;
      }
      default: {
        _GSERR(line, "GS504 - Illegal argument to function 'toupper'");
        break;
      }
    }
  }
}
/*(Standard GScript Library Function) Converts argument string to lowercase and returns
* AST node with it*/
static AST *std_func_tolower(Visitor* visitor, AST **argv, int argc, unsigned line) {
  if(argv[0]->str_value == NULL && argv[0]->func_call_name == NULL && argv[0]->var_name == NULL && argv[0]->type != AST_NUM) {
    _GSERR(line, "GS501 - No arguments provided where needed for function 'tolower'");
    return ast_init(AST_NOOP);
  }
  if (argc != 1) {
    char d[100];
    sprintf(d, "GS502 - Function 'tolower' does not take %d arguments", argc);
    _GSERR(line, d );
    return ast_init(AST_NOOP);
  } else {
    AST* visited_ast = visitor_visit(visitor, argv[0]);
   // printf("%d", visited_ast->type);
    switch(visited_ast->type){
      case AST_STR: {
        AST *lower_ast = ast_init(AST_STR);
        for(int i = 0; i < strlen(visited_ast->str_value); i++){
          if(isalpha(visited_ast->str_value[i])) {
            visited_ast->str_value[i] = tolower(visited_ast->str_value[i]);
          } else {
            visited_ast->str_value[i] = visited_ast->str_value[i];
          }
        }
        lower_ast->str_value = visited_ast->str_value;
        return lower_ast;
      }
      default: {
        _GSERR(line, "GS504 - Illegal argument to function 'tolower'");
        break;
      }
    }
  }
}
static AST *std_func_randnum(Visitor *visitor, AST **argv, int argc, unsigned line) {
  AST *result = ast_init(AST_NUM);
  result->num_value = rand();
  return result;
}
static AST *std_func_num(Visitor *visitor, AST **argv, int argc, unsigned line) {
  if(argv[0]->str_value == NULL && argv[0]->func_call_name == NULL && argv[0]->var_name == NULL && argv[0]->type != AST_NUM) {
    _GSERR(line, "GS501 - No arguments provided where needed for function 'num'");
    return ast_init(AST_NOOP);
  }
  if(argc != 1) {
    char d[100];
    sprintf(d, "GS502 - Function 'num' does not take %d arguments", argc);
    _GSERR(line, d );
    return ast_init(AST_NOOP);
  } else {
    AST* visited_ast = visitor_visit(visitor, argv[0]);
    if(visited_ast->type != AST_STR) _GSERR(line, "GS504 - Illegal argument to function 'num'");
    AST* result = ast_init(AST_NUM);
    result->num_value = strtof(visited_ast->str_value, NULL);
    return result;
  }
}
/*Constructs a new visitor*/
Visitor *visitor_init() {
  Visitor *visitor = calloc(1, sizeof(Visitor));
  visitor->var_defs = (void *)0;
  visitor->var_defs_size = 0;
  visitor->current_scope = scope_init(NULL);
  srand((unsigned)time(NULL));
  return visitor;
}
/*Tells the visitor to visit a syntax tree node*/
AST *visitor_visit(Visitor *visitor, AST *node) {
  switch (node->type) {
  case AST_VAR_DEF:
    return visitor_visit_var_def(visitor, node);
    break;
  case AST_REASSIGN:
    return visitor_visit_var_reassign(visitor, node);
    break;
  case AST_FUNC_DEF:
    return visitor_visit_func_def(visitor, node);
    break;
  case AST_VAR:
    return visitor_visit_var(visitor, node);
    break;
  case AST_FUNC_CALL:
    return visitor_visit_func_call(visitor, node);
    break;
  case AST_STR:
    return visitor_visit_str(visitor, node);
    break;
  case AST_NUM:
    return visitor_visit_num(visitor, node);
    break;
  case AST_BINOP:
    return visitor_visit_binop(visitor, node);
    break;
  case AST_BOOL: 
    return visitor_visit_bool(visitor, node);
    break;
    case AST_IF:
    return visitor_visit_if_statement(visitor, node);
    break;
    case AST_REPEAT:
    return visitor_visit_repeat_statement(visitor, node);
    break;
    case AST_WHILE:
    return visitor_visit_while_statement(visitor, node);
    break;
    case AST_OUT:
      return node;
    break;
    case AST_RETURN:
      return node;
    case AST_UNOP:
    return visitor_visit_unop(visitor, node);
  case AST_COMPOUND:
    return visitor_visit_compound(visitor, node);
    break;
  case AST_NOOP:
    return node;
    break;
  }
  char d[100];
  sprintf(d, "GS201 - Uncaught statement of type %d", node->type);
  _GSERR(node->line, d);
  exit(1);
  return ast_init(AST_NOOP);
}
/*Tells the visitor to interpret a variable definition and store it to memory*/
AST *visitor_visit_var_def(Visitor *visitor, AST *node) {
	AST* val = visitor_visit(visitor, node->var_def_value);
  AST* def = ast_init(AST_VAR_DEF);
  def->var_def_var_name = node->var_def_var_name;
  def->var_def_value = val;
  scope_add_var_def(visitor->current_scope, def);
  return node;
}
AST *visitor_visit_var_reassign(Visitor *visitor, AST *node) {
 // printf("\n ENCOUNTERED REASSIGN with value type %d\n", node->var_def_value->type);
  AST* val = visitor_visit(visitor, node->var_def_value);
 // printf("\nVisitng reassing node, value is %f", val->num_value);
  AST* newval = ast_init(AST_VAR_DEF);
  newval->var_def_var_name = node->var_def_var_name;
  newval->var_def_value = val;
  scope_set_var(visitor->current_scope, newval);
 // printf(" %f ", scope_get_var_def(visitor->current_scope, "a")->num_value);
  return node;
}
AST* visitor_visit_func_def(Visitor *visitor, AST* node) {
  node->scope->parent = visitor->current_scope;
	scope_add_function_definition(node->scope, node);
	return node;
}
/*Tells the visitor to interpret a variable value and store it to memory*/
AST *visitor_visit_var(Visitor *visitor, AST *node) {
  if(strcmp(node->var_name, "pi") == 0) {
    AST* result = ast_init(AST_NUM);
    result->num_value = 3.1415926535898;
    return result;
  }
  AST* val = scope_get_var_def(visitor->current_scope, node->var_name);
  if(val == NULL) {
    char d[100];
    sprintf(d, "GS303 - Variable '%s' does not exist in the current scope", node->var_name);
    _GSERR(node->line, d);
  } else {
    return val;
  }
}
/*Tells the visitor to interpret a function call and execute it*/
AST *visitor_visit_func_call(Visitor *visitor, AST *node) {
//  if(node->should_break) {printf("RETURN") ; exit(1);}
  if (strcmp(node->func_call_name, "write") == 0) {
    return std_func_write(visitor, node->func_call_args,
                          node->func_call_args_size, node->line);
  }
	if (strcmp(node->func_call_name, "read") == 0) {
    return std_func_read(visitor, node->func_call_args,
                         node->func_call_args_size, node->line);
  }
  if (strcmp(node->func_call_name, "toupper") == 0) {
    return std_func_toupper(visitor, node->func_call_args, node->func_call_args_size, node->line);
  }
  if (strcmp(node->func_call_name, "tolower") == 0) {
    return std_func_tolower(visitor, node->func_call_args, node->func_call_args_size, node->line);
  }
  if (strcmp(node->func_call_name, "randnum") == 0) {
    return std_func_randnum(visitor, node->func_call_args, node->func_call_args_size, node->line);
  }
  if (strcmp(node->func_call_name, "num") == 0) {
    return std_func_num(visitor, node->func_call_args, node->func_call_args_size, node->line);
  }
	AST* fdef = scope_get_function_definition(node->scope, node->func_call_name);
	if(fdef != NULL) {
    Scope* prev = visitor->current_scope;
    visitor->current_scope = fdef->scope;
    //printf("%d", fdef->func_def_args_size);
    AST* res = NULL;
    for(int i = 0; i < fdef->func_def_args_size; i++) {
      AST* arg = ast_init(AST_VAR_DEF);
      arg->var_def_var_name = fdef->func_def_args[i]->var_def_var_name;
      arg->var_def_value = visitor_visit(visitor,node->func_call_args[i]);
     // printf("%f", arg->var_def_value->num_value);
     // printf("\n%s", arg->var_def_var_name);
      scope_set_var(fdef->scope, arg);
     // printf("%f", arg->var_def_value->num_value);
    }
    res = visitor_visit(visitor, fdef->func_def_body);
    AST* returned = NULL;
    if(res->return_value) {
      returned = visitor_visit(visitor, res->return_value);
    }
    
    
    visitor->current_scope = prev;
//    if(res->should_break) exit(1);
//    if(res->should_return) exit(1);
		return returned;
	}
  char d[100];
  sprintf(d, "GS503 - Undefined function '%s'", node->func_call_name);
  _GSERR(node->line, d);
  exit(1);
}
/*Tells the visitor to interpret a string AST node*/
AST *visitor_visit_str(Visitor *visitor, AST *node) { return node; }
/*Tells the visitor to begin the recursive process of interpreting the root
 * syntax tree*/
AST *visitor_visit_num(Visitor *visitor, AST *node) {
  return node;
}
AST *visitor_visit_binop(Visitor *visitor, AST *node) {
  AST* left = visitor_visit(visitor, node->left);
  AST* right = visitor_visit(visitor, node->right);
  switch(node->op) {
    case T_PLUS: {
      AST *result = NULL;
      if(left->type == AST_NUM && right->type == AST_NUM) {
        result = ast_init(AST_NUM);
        result->should_break = left->should_break;
        result->num_value = left->num_value + right->num_value;
      } else if(left->type == AST_STR && right->type == AST_STR) {
        result = ast_init(AST_STR);
        result->str_value = left->str_value;
        strcat(result->str_value, right->str_value);
      } else {
        result = ast_init(AST_STR);
        result->str_value = malloc(10 * sizeof(char));
        strcpy(result->str_value, "undefined\0");
      }
      result->line = node->line;
      return result;
    }
    case T_MIN: {
      AST *result = NULL;
      if(left->type == AST_NUM && right->type == AST_NUM) {
        //printf("\nSubbing from %f", left->num_value);
        result = ast_init(AST_NUM);
        result->num_value = left->num_value - right->num_value;
      } else {
        result = ast_init(AST_STR);
        result->str_value = malloc(10 * sizeof(char));
        strcpy(result->str_value, "undefined\0");
      }
      result->line = node->line;
      return result;
    }
    case T_MUL: {
      AST *result = ast_init(AST_NUM);
      result->num_value = left->num_value * right->num_value;
      return result;
    }
    case T_DIV: {
      if(right->num_value == 0) {
        _GSERR(node->line, "GS601 - Dividing by zero is illegal");
      }
      AST *result = ast_init(AST_NUM);
      result->num_value = left->num_value / right->num_value;
      return result;
    }
    case T_MOD: {
      if(right->num_value == 0) {
        _GSERR(right->line, "GS601 - Dividing by zero is illegal");
      }
      AST *result = ast_init(AST_NUM);
      result->num_value = (int)left->num_value % (int)right->num_value;
      return result;
    }
    case T_EE: {
      AST *result = ast_init(AST_BOOL);
      if(left->type == AST_NUM && right->type == AST_NUM) {
        result->b_value = left->num_value == right->num_value;
      } else if(left->type == AST_BOOL && right->type == AST_BOOL) {
        result->b_value = left->b_value == right->b_value;
      } else if(left->type == AST_STR && right->type == AST_STR) {
        result->b_value = (strcmp(left->str_value, right->str_value) == 0);
      }
      return result;
    }
    case T_NEQ: {
      AST *result = ast_init(AST_BOOL);
      if(left->type == AST_NUM && right->type == AST_NUM) {
        result->b_value = left->num_value != right->num_value;
      } else if(left->type == AST_BOOL && right->type == AST_BOOL) {
        result->b_value = left->b_value != right->b_value;
      } else if(left->type == AST_STR && right->type == AST_STR) {
        result->b_value = (strcmp(left->str_value, right->str_value) != 0);
      }
      return result;
    }
    case T_LT: {
      AST *result = ast_init(AST_BOOL);
      result->b_value = left->num_value < right->num_value;
      return result;
    }
    case T_LTE: {
      AST *result = ast_init(AST_BOOL);
      result->b_value = left->num_value <= right->num_value;
      return result;
    }
    case T_GT: {
      AST *result = ast_init(AST_BOOL);
      result->b_value = left->num_value > right->num_value;
      return result;
    }
    case T_GTE: {
      AST *result = ast_init(AST_BOOL);
      result->b_value = left->num_value >= right->num_value;
      return result;
    }
    case T_AND: {
      AST *result = ast_init(AST_BOOL);
      result->b_value = left->b_value && right->b_value;
      return result;
    }
    case T_OR: {
      AST *result = ast_init(AST_BOOL);
      result->b_value = left->b_value || right->b_value;
      return result;
    }
    default: {
      _GSERR(0, "Error while interpreting <INTERNAL>");
      return ast_init(AST_NOOP);
    }
  }
}
AST *visitor_visit_bool(Visitor *visitor, AST *node) {
  return node;
}
AST *visitor_visit_if_statement(Visitor *visitor, AST *node) {
  for(int i = 0; i < node->if_conds_size; i++) {
    AST* cond = visitor_visit(visitor, node->if_conds[i]);
    if(cond->type != AST_BOOL) printf("Expected conditional expression");
    if(cond->b_value) {
      AST* res = visitor_visit(visitor, node->if_bodies[i]);
      node->should_break = res->should_break;
      node->should_return = res->should_return;
      node->return_value = res->return_value;
      return node;
    }
  }
  if(node->else_body != NULL) {
    AST* res = visitor_visit(visitor, node->else_body);
    node->should_break = res->should_break;
      node->should_return = res->should_return;
      node->return_value = res->return_value;
  }
  return node;
}
AST *visitor_visit_repeat_statement(Visitor *visitor, AST *node) {
  AST *count = visitor_visit(visitor, node->repeat_count);
  for(int i = 0; i < count->num_value; i++) {
    AST* stat = visitor_visit(visitor, node->repeat_body);
  //  if(scope_get_var_def(visitor->current_scope, "x")->num_value == 9) printf("ok");
  //  printf(" %f ", scope_get_var_def(visitor->current_scope, "x")->var_def_value->num_value);
    if(stat->should_break) break;
  }
  return ast_init(AST_NOOP);
}
AST *visitor_visit_while_statement(Visitor *visitor, AST *node) {
  while(visitor_visit(visitor, node->while_cond)->b_value) {
    visitor_visit(visitor, node->while_body);
  }
  return ast_init(AST_NOOP);
}
AST *visitor_visit_unop(Visitor *visitor, AST *node) {
  AST* val = visitor_visit(visitor, node->unop_body);
  AST* res = NULL;
  switch(node->unop_op) {
    case T_MIN:
    res = ast_init(AST_NUM);
    res->num_value = -(val->num_value);
    break;
    case T_NOT:
    res = ast_init(AST_BOOL);
    res->b_value = !(val->b_value);
    break;
  }
  return res;
}
AST *visitor_visit_compound(Visitor *visitor, AST *node) {
  Scope* compound_scope = scope_init(visitor->current_scope);
  visitor->current_scope = compound_scope;
  for (int i = 0; i < node->compound_size; i++) {
     AST* stat = visitor_visit(visitor, node->compound_value[i]);
     node->should_break = stat->should_break;
     node->should_return = stat->should_return;
     if(stat->should_return) return stat;
  }
  visitor->current_scope = visitor->current_scope->parent;
  return node;
}