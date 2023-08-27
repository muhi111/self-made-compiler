#!/bin/bash
assert() {
  expected="$1"
  input="$2"

  ./smc "$input" > tmp.s
  cc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

assert 10 'int main(){int i;i=0; while(i<10) { i=i+1; } return i; }'
assert 0 'int main(){return 0;}'
assert 1 'int main(){int i;i=1;return i;}'
assert 0 'int foo(){return 0;}int main(){return foo();}'
assert 3 'int add(int a,int b){return a+b;}int main(){return add(1,2);}'
assert 1 'int main(){int a;a=1;return a;}'
assert 4 'int add(int a,int b){int i;i=1;return a+b+i;}int main(){return add(1,2);}'

assert 32 'int main() { return ret32(); }int ret32() { return 32; }'
assert 7 'int main() { return add2(3,4); }int add2(int x,int y) { return x+y; }'
assert 1 'int main() { return sub2(4,3); }int sub2(int x,int y) { return x-y; }'
assert 55 'int main() { return fib(9); }int fib(int x) {if (x<=1) return 1; return fib(x-1) + fib(x-2); }'

assert 3 'int main(){ int x; int *y; y = &x; *y = 3;return x;}'
assert 3 'int main(){ int x; x=3; return *&x; }'
assert 3 'int main(){ int x; int *y; int *z;x=3; y=&x; z=&y; return **z; }'
assert 5 'int main(){ int x; int *y; x=3; y=&x; *y=5; return x; }'
assert 5 'int main(){ int x; int *y; int *z; x=3;y=&x;z=&y;**z=5; return x;}'

echo OK