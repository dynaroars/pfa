#include <civlc.cvh>

$input int a, b;

// correct version
int gcd_c(int a, int b) {
        if (b == 0) {
            return a;
        } else {
            return gcd_c(b, a%b);
        }
    }

// buggy version
int gcd(int a, int b) {
        if (b == 0) {
            return a;
        } else {
            return gcd(a % b, b);
        }
    }

void main(){
    $assume( 0< a && a < 5 && 0<b && b < 5);
    $assert( gcd_c(a, b) == gcd(a, b) );
}
