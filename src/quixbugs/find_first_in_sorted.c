#include<civlc.cvh>

$input int arr[5];
$input int x;

int find_first_in_sorted_w(int arr[], int x, int length) {
        int lo = 0;
        int hi = length;

        while (lo <= hi) {
            int mid = (lo + hi) / 2;

            if (x == arr[mid] && (mid == 0 || x != arr[mid-1])) {
                return mid;
            } else if (x <= arr[mid]) {
                hi = mid;
            } else {
                lo = mid + 1;
            }
        }
        return -1;
    }

int find_first_in_sorted_c(int arr[], int x, int length) {
        int lo = 0;
        int hi = length;

        while (lo < hi) { // lo <= hi
            int mid = (lo + hi) / 2;

            if (x == arr[mid] && (mid == 0 || x != arr[mid-1])) {
                return mid;
            } else if (x <= arr[mid]) {
                hi = mid;
            } else {
                lo = mid + 1;
            }
        }

        return -1;
    }

void main(){
    for(int i = 0 ; i < 4; i++){
        $assume(arr[i] < arr[i+1]);
    }
    if( find_first_in_sorted_c( arr, x, 5) != find_first_in_sorted_w( arr, x, 5) ){
        $pathCondition();
    }
   // $assert( find_first_in_sorted_c( arr, x, 5) == find_first_in_sorted_w( arr, x, 5) );

}