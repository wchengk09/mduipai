#include <bits/stdc++.h>
#include <unistd.h>
using namespace std;
const int N = 1000;
const int Q = 1000;
mt19937 rnd(getpid());

int main(){
    printf("%d\n",N);
    int ringlen;
    while (1){
        ringlen = rnd() % N + 1;
        if (ringlen > 1 && (ringlen & 1))break;
    }
    for (int i = 1;i <= ringlen;i ++)
        printf("%d %d %d\n",i,i % ringlen + 1,rnd() % 5 + 1);
    for (int i = ringlen + 1;i <= N;i ++)
        printf("%d %d %d\n",rnd() % (i - 1) + 1,i,rnd() % 5 + 1);
    printf("%d\n",Q);
    for (int i = 1;i <= Q;i ++){
        printf("%c %d %d\n",(rnd() & 1) ? 'f' : '?',rnd() % N + 1,rnd() % N + 1);
    }
    return 0;
}