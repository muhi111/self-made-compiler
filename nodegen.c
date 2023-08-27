#include "smc.h"

// 抽象構造木の演算子の部分を作る
Node *new_node(NodeKind kind, Node *lhs, Node *rhs){
	Node *node = calloc(1, sizeof(Node));
	node->kind = kind;
	node->lhs = lhs;
	node->rhs = rhs;
	return node;
}

Node *new_node_ident(void){
	Node *node = calloc(1, sizeof(Node));
	node->kind = ND_LVAR;
	LVar *lvar = find_lvar(token);
	if (lvar){
		node->offset = lvar->offset;
	}else{
		error_at(token->str, "宣言されていない変数が使われました");
	}
	token = token->next;
	return node;
}

Node *new_node_identdef(void){
	Node *node = calloc(1, sizeof(Node));
	node->kind = ND_LVARDEF;
	LVar *lvar = calloc(1, sizeof(LVar));
	lvar->type = calloc(1, sizeof(Type));
	Type *temp = lvar->type;
	while (consume("*")){
		temp->ty = PTR;
		temp->ptr_to = calloc(1, sizeof(Type));
		temp = temp->ptr_to;
		fprintf(stderr, "*がconsumeされました\n");
	}
	temp->ty = INT;
	lvar->prev = locals;
	locals->next = lvar;
	lvar->next = NULL;
	lvar->name = token->str;
	lvar->len = token->len;
	lvar->offset = locals->offset + 8;
	node->offset = lvar->offset;
	locals = lvar;
	token = token->next;
	return node;
}

Node *new_node_func(void){
	Node *node = calloc(1, sizeof(Node));
	node->kind = ND_FUNCDEF;
	node->funcname = calloc((token->len + 1), sizeof(char));
	strncpy(node->funcname, token->str, token->len);
	token = token->next;
	return node;
}

// 抽象構造木の数字の部分を作る
Node *new_node_num(int val){
	Node *node = calloc(1, sizeof(Node));
	node->kind = ND_NUM;
	node->val = val;
	return node;
}