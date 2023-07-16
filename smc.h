#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// トークン
typedef enum{
	TK_RESERVED, // 記号
	TK_NUM,		 // 整数トークン
	TK_EOF,		 // 入力の終わりを表すトークン
	TK_IDENT	 // 識別子
} TokenKind;
typedef struct Token Token;
struct Token{
	TokenKind kind; // トークンの型
	Token *next;	// 次の入力トークン
	int val;		// kindがTK_NUMの場合、その数値
	char *str;		// トークン文字列
	int len;		// トークンの長さ
};

// 抽象構文木
typedef enum{
	ND_ADD,     // +
	ND_SUB,     // -
	ND_MUL,     // *
	ND_DIV,     // /
	ND_EQ,	    // ==
	ND_NE,	    // !=
	ND_LT,	    // <
	ND_LE,	    // <=
	ND_ASSIGN,  // =
	ND_LVAR,    // ローカル変数
	ND_NUM,	    // 整数
} NodeKind;
typedef struct Node Node;
struct Node{
	NodeKind kind; // ノードの型
	Node *lhs;	   // 左辺
	Node *rhs;	   // 右辺
	int val;	   // kindがND_NUMの場合のみ使う
	int offset;	   // kindがND_LVARの場合のみ使う
};

// ローカル変数の型
typedef struct LVar LVar;
struct LVar{
	LVar *next; // 次の変数かNULL
	char *name; // 変数の名前
	int len;	// 名前の長さ
	int offset; // RBPからのオフセット
};

void error_at(char *loc, char *fmt, ...);
bool consume(char *op);
void expect(char *op);
int expect_number(void);
bool at_eof(void);
Token *new_token(TokenKind kind, Token *cur, char *str);
Token *tokenize(void);
Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_ident(void);
Node *new_node_num(int val);
Node *program(void);
Node *stmt(void);
Node *expr(void);
Node *assign(void);
Node *equality(void);
Node *relational(void);
Node *add(void);
Node *mul(void);
Node *unary(void);
Node *primary(void);
void gen_lval(Node *node);
void gen(Node *node);
LVar *find_lvar(Token *tok);

extern Token *token;
// ポインタで宣言してメモリ確保せずに代入する行為って許容されるんだっけ？？？？
extern char *user_input;
extern Node *code[100];
extern LVar *locals;