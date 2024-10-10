#include <bits/stdc++.h>
#define func [&]
using namespace std;
const int MAXN = 2e5 + 5;
int n;
int vcnt;
vector<int> G[MAXN];
int val[MAXN];
int ringid[MAXN],rval[MAXN],rlen,bel[MAXN];
bool vis[MAXN];
int fa[MAXN],dep[MAXN],sz[MAXN],son[MAXN],dfn[MAXN],top[MAXN],rdfn[MAXN],dfn_cnt;

int findring(int x,int fa){
    if (vis[x])return x;
    vis[x] = true;
    for (auto y : G[x]){
        if (y == fa){
            fa = -1;
            continue;
        }
        int ret = findring(y,x);
        if (ret){
            // printf("ringid %d = %d\n",x,rlen + 1);
            ringid[x] = ++rlen;
            rval[rlen] = val[x];
            if (ret == x)return 0;
            return ret;
        }
    }
    return 0;
}

void dfs1(int x,int f){
    sz[x] = 1;
    son[x] = -1;
    int max_sz = -1;
    for (auto y : G[x]){
        if (y == f || ringid[y])continue;
        fa[y] = x;
        dep[y] = dep[x] + 1;
        bel[y] = bel[x];
        dfs1(y,x);
        sz[x] += sz[y];
        if (sz[y] > max_sz){
            max_sz = sz[y];
            son[x] = y;
        }
    }
}

void dfs2(int x,int tp){
    // printf("dfs2 %d (set dfn_cnt %d)\n",x,dfn_cnt + 1);
    dfn[x] = ++dfn_cnt;
    rdfn[dfn_cnt] = x;
    top[x] = tp;
    if (son[x] != -1)
        dfs2(son[x],tp);
    for (auto y : G[x]){
        if (y == fa[x] || y == son[x] || ringid[y])continue;
        dfs2(y,y);
    }
}

int LCA(int x,int y){
    while (top[x] != top[y]){
        if (dep[top[x]] < dep[top[y]])
            swap(x,y);
        x = fa[top[x]];
    }
    if (dep[x] < dep[y])swap(x,y);
    return y;
}

struct Sum{
    int ls,rs,ms;
    Sum(){}
    Sum(int ls,int rs,int ms):ls(ls),rs(rs),ms(ms){};
    inline Sum rev(){return Sum(rs,ls,ms);}
    inline void neg1(){
        ls = -ls;
        rs = -rs;
        ms = -ms;
    }
};

struct Data{
    Sum mins,maxs;
    int sum;
    Data(){}
    Data(Sum mins,Sum maxs,int sum):mins(mins),maxs(maxs),sum(sum){}
    inline Data rev(){return Data(mins.rev(),maxs.rev(),sum);}
    inline void neg1(){mins.neg1(),maxs.neg1(),sum = -sum;}
    inline void print(){printf("DEBUG-DATA: (%d %d %d) (%d %d %d) %d\n",mins.ls,mins.rs,mins.ms,maxs.ls,maxs.rs,maxs.ms,sum);}
};
const Data zero(Sum(0,0,0),Sum(0,0,0),0);

inline Data operator + (Data a,Data b){
    Data c;
    c.mins.ls = min(a.mins.ls,a.sum + b.mins.ls);
    c.mins.rs = min(a.mins.rs + b.sum,b.mins.rs);
    c.mins.ms = min({a.mins.ms,b.mins.ms,a.mins.rs + b.mins.ls});
    c.maxs.ls = max(a.maxs.ls,a.sum + b.maxs.ls);
    c.maxs.rs = max(a.maxs.rs + b.sum,b.maxs.rs);
    c.maxs.ms = max({a.maxs.ms,b.maxs.ms,a.maxs.rs + b.maxs.ls});
    c.sum = a.sum + b.sum;
    return c;
}

inline Data operator += (Data &a,Data b){
    return a = a + b;
}

struct SegTree{
    struct Node{
        int l,r;
        bool lazy;
        Data dt;
    };
    Node T[MAXN << 2];

    inline int lch(int p){return p << 1;}
    inline int rch(int p){return p << 1 | 1;}

    inline void pushup(int p){
        T[p].dt = T[lch(p)].dt + T[rch(p)].dt; 
    }

    void build(int p,int l,int r,function<int(int)> f){
        T[p].l = l,T[p].r = r;
        T[p].lazy = 0;
        if (l == r){
            int v = f(r);
            // printf("at pos %d v=%d\n",r,v);
            T[p].dt = Data(Sum(v,v,v),Sum(v,v,v),v);
            // printf("Data %d [%d,%d]:\n",p,l,r);
            // T[p].dt.print();
            return;
        }
        int mid = (l + r) >> 1;
        build(lch(p),l,mid,f);
        build(rch(p),mid + 1,r,f);
        pushup(p);
        // printf("Data %d [%d,%d]:\n",p,l,r);
        // T[p].dt.print();
    }

    inline void pushdown(int p){
        if (T[p].lazy){
            swap(T[lch(p)].dt.mins,T[lch(p)].dt.maxs);
            swap(T[rch(p)].dt.mins,T[rch(p)].dt.maxs);
            T[lch(p)].dt.neg1();
            T[rch(p)].dt.neg1();
            T[lch(p)].lazy ^= 1;
            T[rch(p)].lazy ^= 1;
            T[p].lazy = 0;
        }
    }

    inline void change(int p,int l,int r){
        if (l > r)return;
        if (l <= T[p].l && T[p].r <= r){
            // T[p].dt.print();
            swap(T[p].dt.mins,T[p].dt.maxs);
            T[p].dt.neg1();
            T[p].lazy ^= 1;
            // T[p].dt.print();
            return;
        }
        pushdown(p);
        int mid = (T[p].l + T[p].r) >> 1;
        if (l <= mid)change(lch(p),l,r);
        if (r > mid)change(rch(p),l,r);
        pushup(p);
        // printf("%d [%d,%d]: ",p,T[p].l,T[p].r);
        // T[p].dt.print();
    }

    Data query(int p,int l,int r){
        // if (p == 1)printf("query %d %d\n",l,r);
        if (l > r)return zero;
        if (l <= T[p].l && T[p].r <= r)
            return T[p].dt;
        pushdown(p);
        int mid = (T[p].l + T[p].r) >> 1;
        Data ret = zero;
        if (l <= mid)ret += query(lch(p),l,r);
        if (r > mid)ret += query(rch(p),l,r);
        return ret;
    }

    void print(int p){
        printf("%d [%d,%d] ",p,T[p].l,T[p].r);
        T[p].dt.print();
        if (T[p].l == T[p].r)return;
        pushdown(p);
        print(lch(p));
        print(rch(p));
    }
};
SegTree ring,tree;

int main(){
    scanf("%d",&n);
    vcnt = n;
    for (int i = 1;i <= n;i ++){
        int x,y,z;
        scanf("%d%d%d",&x,&y,&z);
        ++vcnt;
        G[vcnt].push_back(x);
        G[x].push_back(vcnt);
        G[vcnt].push_back(y);
        G[y].push_back(vcnt);
        val[vcnt] = z;
    }
    findring(1,-1);
    
    for (int i = 1;i <= vcnt;i ++)
        if (ringid[i])
            bel[i] = i,dfs1(i,-1),dfs2(i,i);
    tree.build(1,1,dfn_cnt,func(int idx){return val[rdfn[idx]];});
    ring.build(1,1,rlen,func(int idx){return rval[idx];});
    // for (int i = 1;i <= vcnt;i ++)
    //     printf("dfn %d = %d\n",i,dfn[i]);
    // for (int i = 1;i <= vcnt;i ++)
    //     printf("bel %d = %d\n",i,bel[i]);
    // for (int i = 1;i <= vcnt;i ++)
    //     printf("val %d = %d\n",i,val[i]);
    // for (int i = 1;i <= vcnt;i ++)
    //     printf("ringid %d = %d\n",i,ringid[i]);
    // for (int i = 1;i <= n;i ++)
    //     for (auto j : G[i]){
    //         if (j > n)printf("%d %d(%d)\n",i,j,val[j]);
    //         else printf("%d %d\n",i,j);
    //     }
    // for (int i = 1;i <= vcnt;i ++)
    //     printf("dfn %d = %d\n",i,dfn[i]);
    int q;
    scanf("%d",&q);
    while (q --){
        char op[2];
        scanf("%s",op);
        if (op[0] == '?'){
            int x,y;
            scanf("%d%d",&x,&y);
            if (bel[x] == bel[y]){
                int lca = LCA(x,y);
                Data ans1 = zero;
                while (top[x] != top[lca]){
                    int tp = top[x];
                    ans1 += tree.query(1,dfn[tp],dfn[x]).rev();
                    x = fa[tp];
                }
                ans1 += tree.query(1,dfn[lca],dfn[x]).rev();
                Data ans2 = zero;
                while (top[y] != top[lca]){
                    int tp = top[y];
                    ans2 = tree.query(1,dfn[tp],dfn[y]) + ans2;
                    y = fa[tp];
                }
                ans1 = ans1 + tree.query(1,dfn[lca],dfn[y]) + ans2;
                printf("%d\n",max(ans1.maxs.ms,0));
            }else{
                Data ans1 = zero,ans2 = zero;
                int tpx = bel[x],tpy = bel[y];
                while (x){
                    ans1 = ans1 + tree.query(1,dfn[top[x]],dfn[x]).rev();
                    // printf("query %d %d\n",top[x],x);
                    x = fa[top[x]];
                }
                // printf("ans1: ");
                // ans1.print();
                while (y){
                    ans2 = tree.query(1,dfn[top[y]],dfn[y]) + ans2;
                    y = fa[top[y]];
                }
                // printf("ans2: ");
                // ans2.print();
                // printf("ans2: %d %d %d %d\n",ans2.maxs.ls,ans2.maxs.rs,ans2.maxs.ms,ans2.sum);
                // printf("tpx %d id %d tpy %d id %d\n",tpx,ringid[tpx],tpy,ringid[tpy]);
                int len = abs(ringid[tpy] - ringid[tpx]);
                if (ringid[tpx] < ringid[tpy]){
                    if ((len << 1) <= rlen){
                        Data ret = ring.query(1,ringid[tpx] + 1,ringid[tpy] - 1);
                        ans1 = ans1 + ret;
                        // printf("query_ring %d %d\n",ringid[tpx] + 1,ringid[tpy] - 1);
                        // ret.print();
                    }else{
                        ans1 = ans1 + ring.query(1,1,ringid[tpx] - 1).rev();
                        ans1 = ans1 + ring.query(1,ringid[tpy] + 1,rlen).rev();
                    }
                }else{
                    if ((len << 1) <= rlen){
                        ans1 = ans1 + ring.query(1,ringid[tpy] + 1,ringid[tpx] - 1).rev();
                        // printf("query_ring %d %d\n",ringid[tpy] + 1,ringid[tpx] - 1);
                    }
                    else{
                        // printf("query_ring %d %d\n",ringid[tpx] + 1,rlen);
                        ans1 = ans1 + ring.query(1,ringid[tpx] + 1,rlen);
                        // printf("query_ring %d %d\n",1,ringid[tpy] - 1);
                        ans1 = ans1 + ring.query(1,1,ringid[tpy] - 1);
                    }
                }
                ans1 = ans1 + ans2;
                printf("%d\n",max(ans1.maxs.ms,0));
            }
        }
        if (op[0] == 'f'){
            int x,y;
            scanf("%d%d",&x,&y);
            if (bel[x] == bel[y]){
                int lca = LCA(x,y);
                while (top[x] != top[lca]){
                    int tp = top[x];
                    tree.change(1,dfn[tp],dfn[x]);
                    x = fa[tp];
                }
                tree.change(1,dfn[lca],dfn[x]);
                while (top[y] != top[lca]){
                    int tp = top[y];
                    tree.change(1,dfn[tp],dfn[y]);
                    y = fa[tp];
                }
                tree.change(1,dfn[lca],dfn[y]);
            }else{
                int tpx = bel[x],tpy = bel[y];
                // printf("tpx %d tpy %d\n",tpx,tpy);
                while (x){
                    tree.change(1,dfn[top[x]],dfn[x]);
                    // printf("change %d %d\n",dfn[top[x]],dfn[x]);
                    x = fa[top[x]];
                }
                while (y){
                    tree.change(1,dfn[top[y]],dfn[y]);
                    // printf("change %d %d\n",dfn[top[y]],dfn[y]);
                    y = fa[top[y]];
                }
                int len = abs(ringid[tpy] - ringid[tpx]) + 1;
                if (ringid[tpx] < ringid[tpy]){
                    if ((len << 1) <= rlen){
                        // printf("change_ring %d %d\n",ringid[tpx] + 1,ringid[tpy] - 1);
                        ring.change(1,ringid[tpx] + 1,ringid[tpy] - 1);
                    }
                    else{
                        ring.change(1,1,ringid[tpx] - 1);
                        ring.change(1,ringid[tpy] + 1,rlen);
                    }
                }else{
                    if ((len << 1) <= rlen){
                        // printf("change_ring %d %d\n",ringid[tpy] + 1,ringid[tpx] - 1);
                        ring.change(1,ringid[tpy] + 1,ringid[tpx] - 1);
                        // ring.print(1);
                    }
                    else{
                        ring.change(1,ringid[tpx] + 1,rlen);
                        ring.change(1,1,ringid[tpy] - 1);
                    }
                }
            }
        }
        // printf("TREE:\n");
        // tree.print(1);
        // printf("RING:\n");
        // ring.print(1);
    }
    return 0;
}