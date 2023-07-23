#!/bin/bash
assert() {
  expected="$1"
  input="$2"

  ./smc "$input" > tmp.s
  cc -o tmp tmp.s foo.c
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

assert 0 '{return 0;}'
assert 2 '{ if (2-1) return 2; return 3; }'
assert 4 '{ if (0) { 1; 2; return 3; } else { return 4; } }'
assert 3 '{ if (1) { 1; 2; return 3; } else { return 4; } }'
assert 55 '{ i=0; j=0; for (i=0; i<=10; i=i+1) j=i+j; return j; }'

assert 10 '{ i=0; while(i<10) { i=i+1; } return i; }'
assert 3 'return foo(3);'
assert 5 'return add(2, 3);'
# assert 5 'return add(sub(3,1),3);'
assert 21 'return add_6(1, 2, 3, 4, 5, 6);'
# assert 0 '{{a=1;b=2}return 3;}'

echo OK