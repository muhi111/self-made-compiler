#include "smc.h"

// program = stmt*
Node *program(void){
	int i = 0;
	while (!at_eof()){
		code[i] = stmt();
		i++;
	}
	code[i] = NULL;
}
// stmt = expr ";"
Node *stmt(void){
	Node *node = expr();
	expect(";");
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
// unary = ("+" | "-")? primary
Node *unary(void){
	if (consume("+")){
		return primary();
	}else if (consume("-")){
		return new_node(ND_SUB, new_node_num(0), primary());
	}
	return primary();
}
// primary = num | ident | "(" expr ")"
Node *primary(void){
	if (consume("(")){
		Node *node = expr();
		expect(")");
		return node;
	}else if(token->kind == TK_IDENT){
		return new_node_ident(token->str[0]);
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

Node *new_node_ident(char ident){
	Node *node = calloc(1, sizeof(Node));
	node->kind = ND_LVAR;
	node->offset = (ident - 'a' + 1) * 8;
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
// 複数文字の演算子に対応
// memcmpじゃなくstrncmpとかでよくね？？？？
bool consume(char *op){
	if (token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len) != 0)
		return false;
	token = token->next;
	return true;
}

// やってること自体はconsumeと同じ,TFを返すかどうかが違い
// 複数文字の演算子に対応
void expect(char *op){
	if (token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len) != 0)
		error_at(token->str,"'%s'ではありません", op);
	token = token->next;
}

// 数字が来ると想定される場所で(演算子の後)、数字を返す
int expect_number(void){
	if (token->kind != TK_NUM)
		printf(token->str, "数ではありません");
	int val = token->val;
	token = token->next;
	return val;
}

// 最後のtokenかを判別する
bool at_eof(void){
	return token->kind == TK_EOF;
}

// 新しいtokenを作る
Token *new_token(TokenKind kind, Token *cur, char *str, int len){
	Token *tok = calloc(1, sizeof(Token));
	tok->kind = kind;
	tok->str = str;
	tok->len = len;
	cur->next = tok;
	return tok;
}

// スペースを飛ばして式をtoken化して線形リストに格納する
Token *tokenize(void){
	char *p = user_input;
	Token head;
	head.next = NULL;
	Token *cur = &head;

	while (*p){
		if (isspace(*p)){
			p++;
			continue;
		}
		if (strncmp(p,"<=",2) == 0 || strncmp(p,">=",2) == 0 || strncmp(p,"==",2) == 0 || strncmp(p,"!=",2) == 0){
			cur = new_token(TK_RESERVED, cur, p, 2);
			p += 2;
			continue;
		}
		else if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')' || *p == '>' || *p == '<' || *p == ';' || *p == '=')
		{
			cur = new_token(TK_RESERVED, cur, p, 1);
			p++;
			continue;
		}
		else if ('a' <= *p && *p <= 'z')
		{
			cur = new_token(TK_IDENT, cur, p, 1);
			p++;
			continue;
		}
		else if (isdigit(*p))
		{
				cur = new_token(TK_NUM, cur, p, 0);
				cur->val = strtol(p, &p, 10);
				continue;
		}
		error_at(p,"トークナイズできません");
	}
	new_token(TK_EOF, cur, p, 0);
	token =  head.next;
}