#include <civlc.cvh>

$input int n;

int bitcount_c(int n) {
    int count = 0;
    while (n != 0) {
        n = (n & (n - 1));
        count++;
    }
    return count;
    }

int bitcount_w(int n) {
    int count = 0;
    while (n != 0) {
        n = (n ^ (n - 1));
        count++;
    }
    return count;
    }

int main(){
    $assume( 0 < n && n < 3);
    $assert(bitcount_c(n) == bitcount_w(n));
}