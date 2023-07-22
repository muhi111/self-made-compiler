#include "smc.h"

// program = stmt*
Node *program(void){
	Node head = {};
	Node *cur = &head;
	while (!at_eof()){
		cur->next = stmt();
		cur = cur->next;
	}
	cur = calloc(1, sizeof(Node));
	cur = NULL;
	return head.next;
}
// stmt = expr ";" | "return" expr ";" | "{" stmt* "}" | "if" "(" expr ")" stmt ("else" stmt)?
//       | "while" "(" expr ")" stmt | "for" "(" expr ";" expr ";" expr ")" stmt
Node *stmt(void){
	Node *node;
	if(consume("return")){
		node = new_node(ND_RETURN, expr(), NULL);
		expect(";");
	}else if(consume("{")){
		int i = 0;
		Node head = {};
		Node *cur = &head;
		node = new_node(ND_BLOCK, NULL, NULL);
		while (1){
			if (consume("}")){
				cur = calloc(1, sizeof(Node));
				cur = NULL;
				node->block_next = head.block_next;
				break;
			}
			cur->block_next = stmt();
			cur = cur->block_next;
		}
	}else if(consume("if")){
		node = new_node(ND_IF, NULL, NULL);
		expect("(");
		node->cond = expr();
		expect(")");
		node->then = stmt();
		if(consume("else")){
			node->els = stmt();
		}
	}else if(consume("while")){
		node = new_node(ND_WHILE, NULL, NULL);
		expect("(");
		node->cond = expr();
		expect(")");
		node->then = stmt();
	}else if(consume("for")){
		node = new_node(ND_FOR, NULL, NULL);
		expect("(");
		node->init = expr();
		expect(";");
		node->cond = expr();
		expect(";");
		node->inc = expr();
		expect(")");
		node->then = stmt();
	}else{
		node = expr();
		expect(";");
	}
	return node;
}
// expr = assign
Node *expr(void){
	Node *node = assign();
	return node;
}
// assign = equality ("=" assign)?
Node *assign(void){
	Node *node = equality();
	if (consume("=")){
		node = new_node(ND_ASSIGN, node, assign());
	}
	return node;
}
// equality   = relational ("==" relational | "!=" relational)*
Node *equality(void){
	Node *node = relational();
	while (1){
		if(consume("==")){
			node = new_node(ND_EQ, node, relational());
		}else if (consume("!=")){
			node = new_node(ND_NE, node, relational());
		}else{
			return node;
		}
	}
}
// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
Node *relational(void){
	Node *node = add();
	while (1){
		if (consume("<")){
			node = new_node(ND_LT, node, add());
		}else if (consume("<=")){
			node = new_node(ND_LE, node, add());
		}else if(consume(">")){
			node = new_node(ND_LT, add(), node);
		}else if(consume(">=")){
			node = new_node(ND_LE, add(), node);
		}else{
			return node;
		}
	}
}
// add = mul ("+" mul | "-" mul)*
Node *add(void){
	Node *node = mul();
	while (1){
		if (consume("+")){
			node = new_node(ND_ADD, node, mul());
		}else if (consume("-")){
			node = new_node(ND_SUB, node, mul());
		}else{
			return node;
		}
	}
}
// mul = unary ("*" unary | "/" unary)*
Node *mul(void){
	Node *node = unary();
	while(1){
		if (consume("*")){
			node = new_node(ND_MUL, node, unary());
		}else if (consume("/")){
			node = new_node(ND_DIV, node, unary());
		}else{
			return node;
		}
	}
}
// unary = ("+" | "-")* primary
Node *unary(void){
	if (consume("+")){
		return unary();
	}else if (consume("-")){
		return new_node(ND_SUB, new_node_num(0), unary());
	}
	return primary();
}
// primary = num | ident("(" ")")? | "(" expr ")"
Node *primary(void){
	Node *node;
	if (consume("(")){
		node = expr();
		expect(")");
		return node;
	}else if(token->kind == TK_IDENT){
		if(token->next->str[0] == '('){
			node = new_node(ND_FUNCCALL, NULL, NULL);
			node->funcname = malloc(sizeof(char) * (token->len + 1));
			strncpy(node->funcname, token->str, token->len);
			token = token->next;
			expect("(");
			expect(")");
			return node;
		}
		return new_node_ident();
	}
	return new_node_num(expect_number());
}

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
		lvar = calloc(1, sizeof(LVar));
		lvar->next = locals;
		lvar->name = token->str;
		lvar->len = token->len;
		lvar->offset = locals->offset + 8;
		node->offset = lvar->offset;
		locals = lvar;
	}
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

// エラーとその場所を表示
void error_at(char *loc, char *fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	int pos = loc - user_input;
	fprintf(stderr, "%s\n", user_input);
	fprintf(stderr, "%*s", pos, " ");
	fprintf(stderr, "^ ");
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	va_end(ap);
	exit(1);
}

// 演算子の場合tokenを進める
bool consume(char *op){
	if ((token->kind != TK_RESERVED && token->kind != TK_CONTROL) || strlen(op) != token->len || memcmp(token->str, op, token->len) != 0){
		return false;
	}
	token = token->next;
	return true;
}

// やってること自体はconsumeと同じ,TFを返すかどうかが違い
void expect(char *op){
	if (token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len) != 0)
		error_at(token->str,"'%s'ではありません", op);
	token = token->next;
}

// 数字が来ると想定される場所で(演算子の後)、数字を返す
int expect_number(void){
	if (token->kind != TK_NUM)
		error_at(token->str, "数ではありません");
	int val = token->val;
	token = token->next;
	return val;
}

// 最後のtokenかを判別する
bool at_eof(void){
	return token->kind == TK_EOF;
}

LVar *find_lvar(Token *tok){
	for (LVar *var = locals; var; var = var->next)
		if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
			return var;
	return NULL;
}
