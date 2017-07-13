#include<iostream>
#include<vector>
#include<algorithm>
#include<cstdio>
#include<queue>
#include<cmath>
#include<map>
#include<set>
#include "verify.h"
using namespace std;
const int INF = 1 << 29;
const double EPS = 1e-9;

typedef long long ll;

#define SORT(c) sort((c).begin(),(c).end())
#define FOR(i,a,b) for(int i=(a);i<(b);++i)
#define rep(i,n)  FOR(i,0,n)

void run_test(string describe, int flag) {
    cout << describe << "...";
    if (flag) {
        cout << "OK" << endl;
    }
    else {
        cout << "NG" << endl;
    }
}

int main(int argc, char * argv[])
{
    ios::sync_with_stdio(false);
    int flag = test_id();
    run_test("run test id matching", flag);


    return 0;
}


