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

assert 10 'main(){ i=0; while(i<10) { i=i+1; } return i; }'
assert 0 'main(){return 0;}'
assert 1 'main(){i=1;return i;}'
assert 0 'foo(){return 0;}main(){return foo();}'
assert 6 'foo(){i=1;j=2;k=3;return i+j+k;}main(){a=foo();i=10;j=20;k=30;return a;}'
assert 3 'add(a,b){return a+b;}main(){return add(1,2);}'
assert 1 'main(){a=1;return a;}'
assert 4 'add(a,b){i=1;return a+b+i;}main(){return add(1,2);}'

assert 32 'main() { return ret32(); }ret32() { return 32; }'
assert 7 'main() { return add2(3,4); }add2(x,y) { return x+y; }'
assert 1 'main() { return sub2(4,3); }sub2(x,y) { return x-y; }'
assert 55 'main() { return fib(9); }fib(x) { if (x<=1) return 1; return fib(x-1) + fib(x-2); }'

assert 3 'main(){x = 3;y = &x;return *y;}'
assert 3 'main(){ x=3; return *&x; }'
assert 3 'main(){ x=3; y=&x; z=&y; return **z; }'
assert 5 'main(){ x=3; y=&x; *y=5; return x; }'
assert 5 'main(){ x=3;y=&x;z=&y;**z=5; return x;}'

echo OK