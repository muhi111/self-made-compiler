#include "smc.h"

char argreg[6][4] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

void gen_lval(Node *node){
	if (node->kind != ND_LVAR){
		fprintf(stderr, "代入の左辺値が変数ではありません\n");
	}
	printf("  mov rax, rbp\n");
	printf("  sub rax, %d\n", node->offset);
	printf("  push rax\n");
}

void gen(Node *node){
	switch (node->kind){
	case ND_RETURN:
		gen(node->lhs);
		printf("  pop rax\n");
		printf("  mov rsp, rbp\n");
		printf("  pop rbp\n");
		printf("  ret\n");
		return;
	case ND_BLOCK:
		node = node->block;
		while(node){
			gen(node);
			node = node->block_next;
			printf("  pop rax\n");
		}
		return;
	case ND_IF:
		gen(node->cond);
		printf("  pop rax\n");
		printf("  cmp rax, 0\n");
		if(node->els){
			printf("  je  .Lelse%d\n", uuid);
			gen(node->then);
			printf("  jmp .Lend%d\n", uuid + 1);
			printf(".Lelse%d:\n", uuid);
			gen(node->els);
			printf(".Lend%d:\n", uuid + 1);
			uuid += 2;
		}else{
			printf("  je  .Lend%d\n", uuid);
			gen(node->then);
			printf(".Lend%d:\n", uuid);
			uuid += 1;
		}
		return;
	case ND_WHILE:
		printf(".Lbegin%d:\n", uuid);
		gen(node->cond);
		printf("  pop rax\n");
		printf("  cmp rax, 0\n");
		printf("  je  .Lend%d\n", uuid + 1);
		gen(node->then);
		printf("  jmp .Lbegin%d\n", uuid);
		printf(".Lend%d:\n", uuid + 1);
		uuid += 2;
		return;
	case ND_FOR:
		gen(node->init);
		printf(".Lbegin%d:\n", uuid);
		gen(node->cond);
		printf("  pop rax\n");
		printf("  cmp rax, 0\n");
		printf("  je  .Lend%d\n", uuid + 1);
		gen(node->then);
		gen(node->inc);
		printf("  jmp .Lbegin%d\n", uuid);
		printf(".Lend%d:\n", uuid + 1);
		uuid += 2;
		return;
	case ND_FUNCCALL:
		Node *node_temp = node->args;
		int i = 0;
		while (node_temp){
			gen(node_temp);
			printf("  pop %s\n", argreg[i]);
			node_temp = node_temp->args_next;
			i++;
		}
		printf("  call %s\n", node->funcname);
		printf("  push rax\n");
		return;
	case ND_NUM:
		printf("  push %d\n", node->val);
		return;
	case ND_LVAR:
		gen_lval(node);
		printf("  pop rax\n");
		printf("  mov rax, [rax]\n");
		printf("  push rax\n");
		return;
	case ND_ASSIGN:
		gen_lval(node->lhs);
		gen(node->rhs);
		printf("  pop rdi\n");
		printf("  pop rax\n");
		printf("  mov [rax], rdi\n");
		printf("  push rdi\n");
		return;
	}

	gen(node->lhs);
	gen(node->rhs);

	printf("  pop rdi\n");
	printf("  pop rax\n");

	switch (node->kind){
	case ND_ADD:
		printf("  add rax, rdi\n");
		break;
	case ND_SUB:
		printf("  sub rax, rdi\n");
		break;
	case ND_MUL:
		printf("  imul rax, rdi\n");
		break;
	case ND_DIV:
		printf("  cqo\n");
		printf("  idiv rdi\n");
		break;
	case ND_EQ:
		printf("  cmp rax, rdi\n");
		printf("  sete al\n");
		printf("  movzb rax, al\n");
		break;
	case ND_NE:
		printf("  cmp rax, rdi\n");
		printf("  setne al\n");
		printf("  movzb rax, al\n");
		break;
	case ND_LT:
		printf("  cmp rax, rdi\n");
		printf("  setl al\n");
		printf("  movzb rax, al\n");
		break;
	case ND_LE:
		printf("  cmp rax, rdi\n");
		printf("  setle al\n");
		printf("  movzb rax, al\n");
		break;
	}
	printf("  push rax\n");
}