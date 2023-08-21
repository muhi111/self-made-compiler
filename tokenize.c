#include "smc.h"

// 新しいtokenを作る
Token *new_token(TokenKind kind, Token *cur, char *str){
	Token *tok = calloc(1, sizeof(Token));
	tok->kind = kind;
	tok->str = str;
	cur->next = tok;
	return tok;
}

// スペースを飛ばして式をtoken化して線形リストに格納する
void *tokenize(void){
	char *p = user_input;
	Token head = {};
	Token *cur = &head;

	while (*p){
		if (isspace(*p)){
			p++;
			continue;
		}else if ( strncmp(p,"<=",2) == 0|| strncmp(p,">=",2) == 0|| strncmp(p,"==",2) == 0|| strncmp(p,"!=",2) == 0){
			cur = new_token(TK_RESERVED, cur, p);
			cur->len = 2;
			p += 2;
			continue;
		}else if (ispunct(*p)){
			if(*p == '{' || *p == '}'){
				cur = new_token(TK_CONTROL, cur, p);
			}else{
				cur = new_token(TK_RESERVED, cur, p);
			}
			cur->len = 1;
			p++;
			continue;
		}else if (strncmp(p, "return", 6) == 0 && isspace(p[6])){
			cur = new_token(TK_CONTROL, cur, p);
			cur->len = 6;
			p += 6;
			continue;
		}else if(strncmp(p, "if", 2) == 0){
			cur = new_token(TK_CONTROL, cur, p);
			cur->len = 2;
			p += 2;
			continue;
		}else if(strncmp(p, "else", 4) == 0){
			cur = new_token(TK_CONTROL, cur, p);
			cur->len = 4;
			p += 4;
			continue;
		}else if(strncmp(p, "while", 5) == 0){
			cur = new_token(TK_CONTROL, cur, p);
			cur->len = 5;
			p += 5;
			continue;
		}else if(strncmp(p, "for", 3) == 0){
			cur = new_token(TK_CONTROL, cur, p);
			cur->len = 3;
			p += 3;
			continue;
		}else if(strncmp(p, "int", 3) == 0){
			cur = new_token(TK_CONTROL, cur, p);
			cur->len = 3;
			p += 3;
		}else if (isalpha(*p)){
			int len = 0;
			do{
				len++;
				p++;
			} while (isalpha(*p) || isdigit(*p) || *p == '_');
			cur = new_token(TK_IDENT, cur, p - len);
			cur->len = len;
			continue;
		}else if (isdigit(*p)){
			cur = new_token(TK_NUM, cur, p);
			cur->val = strtol(p, &p, 10);
			continue;
		}
	}
	new_token(TK_EOF, cur, p);
	token =  head.next;
}