#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Token Token;
typedef struct Node Node;
typedef struct LVar LVar;

// トークン
typedef enum{
	TK_RESERVED, // 記号
	TK_NUM,		 // 整数トークン
	TK_EOF,		 // 入力の終わりを表すトークン
	TK_IDENT,	 // 識別子
	TK_CONTROL,  // 制御構造
} TokenKind;
struct Token{
	TokenKind kind; // トークンの型
	Token *next;	// 次の入力トークン
	int val;		// kindがTK_NUMの場合、その数値
	char *str;		// トークン文字列
	int len;		// トークンの長さ
};

// 抽象構文木
typedef enum{
	ND_ADD,	   // +
	ND_SUB,	   // -
	ND_MUL,	   // *
	ND_DIV,	   // /
	ND_EQ,	   // ==
	ND_NE,	   // !=
	ND_LT,	   // <
	ND_LE,	   // <=
	ND_ASSIGN, // =
	ND_LVAR,   // ローカル変数
	ND_LVARDEF,
	ND_NUM,	   // 整数
	ND_RETURN,
	ND_BLOCK,
	ND_IF,
	ND_WHILE,
	ND_FOR,
	ND_FUNCCALL,
	ND_FUNCDEF,
	ND_ADDR,
	ND_DEREF,
} NodeKind;
struct Node{
	NodeKind kind;     // ノードの型
	Node *lhs;	       // 左辺
	Node *rhs;	       // 右辺
	Node *block;       // ブロックで分岐するためのもの
	Node *block_next;  // ブロック内での次のstmtを指す
	Node *next;        // 次のstmtを指す
	Node *cond;	       // if,while,forで使う
	Node *then;        // if,while,forで使う
	Node *els;         // ifで使う
	Node *init;        // forで使う
	Node *inc;         // forで使う
	Node *args;        // 関数をの引数を読み込むためのもの ND_FUNCCALLで使う
	Node *args_next;   // 次の関数の引数を表わす ND_FUNCCALLで使う
	Node *func_next;   // 次の関数の宣言を表わす ND_FUNCDEFで使う
	LVar *lvar;	       // 関数の引数も含むローカル変数を格納する ND_FUNDEFで使う
	char *funcname;    // 関数の名前
	int val;		   // kindがND_NUMの場合のみ使う
	int offset;	       // kindがND_LVARの場合のみ使う
};

// ローカル変数の型
struct LVar{
	LVar *next;      // 次の変数かNULL
	LVar *prev;      // 前の変数かNULL
	LVar *next_func; // 次の関数の変数
	char *name;      // 変数の名前
	int len;	     // 名前の長さ
	int offset;      // RBPからのオフセット
	int arg_flag;    // 関数の引数であることを示すフラグ　0以外なら引数
};

void error_at(char *loc, char *fmt, ...);
bool consume(char *op);
void expect(char *op);
int expect_number(void);
bool at_eof(void);
Token *new_token(TokenKind kind, Token *cur, char *str);
void *tokenize(void);
Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_ident(void);
Node *new_node_identdef(void);
Node *new_node_num(int val);
Node *program(void);
Node *function();
Node *stmt(void);
Node *expr(void);
Node *assign(void);
Node *equality(void);
Node *relational(void);
Node *add(void);
Node *mul(void);
Node *unary(void);
Node *primary(void);
Node *func_call(void);
Node *new_node_func(void);
void gen_lval(Node *node);
void gen(Node *node);
LVar *find_lvar(Token *tok);

extern Token *token;
extern char *user_input;
extern LVar *locals;
extern int uuid;