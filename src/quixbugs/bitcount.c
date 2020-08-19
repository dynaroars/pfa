#include <civlc.cvh>

$input int n;

int bitcount_w(int n) {
    int count = 0;
    while (n != 0) {
        n = (n ^ (n - 1));
        if (count > 10)
            return -1;
        count++;
    }
    return count;
}

int main(){
    $assume( -1 < n );
    if (bitcount_w(n) == -1) {
        $pathCondition();
    }
    //$assert(bitcount_c(n) == bitcount_w(n));
}