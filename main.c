#include "smc.h"

char *user_input;
Token *token;
LVar *locals;
int uuid;

int main(int argc, char **argv){
	if (argc != 2){
		fprintf(stderr, "引数の個数が正しくありません");
		return 1;
	}

	uuid = 0;
	user_input = argv[1];
	tokenize();
	locals = calloc(1, sizeof(LVar));
	Node *node = program();

	// アセンブリの前半部分を出力
	printf(".intel_syntax noprefix\n");
	printf(".globl main\n");
	printf("main:\n");

	// プロローグ
	// 変数26個分の領域を確保する
	printf("  push rbp\n");
	printf("  mov rbp, rsp\n");
	printf("  sub rsp, 208\n");

	while(node){
		gen(node);
		node = node->next;

		// 式の評価結果としてスタックに一つの値が残っている
		// はずなので、スタックが溢れないようにポップしておく
		printf("  pop rax\n");
	}

	// エピローグ
	// 最後の式の結果がRAXに残っているのでそれが返り値になる
	printf("  mov rsp, rbp\n");
	printf("  pop rbp\n");
	printf("  ret\n");
	return 0;
}