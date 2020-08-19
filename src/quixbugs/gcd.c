#include <civlc.cvh>

$input int a, b;

int gcd(int a, int b, int count) {
    if (count > 100)
        return -1;
    if (b == 0) {
        return a;
    } else {
        return gcd(a % b, b, count + 1);
    }
}

void main(){
    //$assume( 0< a && a < 5 && 0<b && b < 5);
    if (gcd(a, b, 0) == -1) {
        $pathCondition();
    }
}
