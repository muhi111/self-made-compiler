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

assert 0 'main(){return 0;}'
assert 2 'main(){ if (2-1) return 2; return 3; }'
assert 4 'main(){ if (0) { 1; 2; return 3; } else { return 4; } }'
assert 3 'main(){ if (1) { 1; 2; return 3; } else { return 4; } }'
assert 55 'main(){ i=0; j=0; for (i=0; i<=10; i=i+1) j=i+j; return j; }'

assert 10 'main(){ i=0; while(i<10) { i=i+1; } return i; }'
assert 0 'main(){return 0;}'
assert 1 'main(){i=1;return i;}'
assert 0 'foo(){return 0;}main(){return foo();}'
assert 6 'foo(){i=1;j=2;k=3;return i+j+k;}main(){a=foo();i=10;j=20;k=30;return a;}'

echo OK