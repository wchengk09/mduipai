#include <bits/stdc++.h>
#define fi first
#define se second
#define pii pair<int,int>
#define pdi pair<db,int>
#define mp make_pair
#define pb push_back
#define enter putchar('\n')
#define space putchar(' ')
#define eps 1e-8
#define mo 974711
#define MAXN 100005
//#define ivorysi
using namespace std;
typedef long long int64;
typedef double db;
template<class T>
void read(T &res) {
    res = 0;char c = getchar();T f = 1;
    while(c < '0' || c > '9') {
	if(c == '-') f = -1;
	c = getchar();
    }
    while(c >= '0' && c <= '9') {
	res = res * 10 + c - '0';
	c = getchar();
    }
    res *= f;
}
template<class T>
void out(T x) {
    if(x < 0) {x = -x;putchar('-');}
    if(x >= 10) {
	out(x / 10);
    }
    putchar('0' + x % 10);
}
int N,M;
struct node {
    int to,next;int64 val;
}E[MAXN * 2];
int head[MAXN],sumE;
int fa[MAXN],dfn[MAXN],siz[MAXN],dep[MAXN],idx,son[MAXN],top[MAXN],seq[MAXN];
int md[MAXN * 2][20],len[MAXN * 2],pos[MAXN],bud;

int st,ed;int64 ev;
int64 worth[MAXN];
bool vis[MAXN];
void add(int u,int v,int64 c) {
    E[++sumE].to = v;
    E[sumE].next = head[u];
    E[sumE].val = c;
    head[u] = sumE;
}
int min_dep(int a,int b) {
    return dep[a] < dep[b] ? a : b;
}
int lca(int a,int b) {
    a = pos[a];b = pos[b];
    if(a > b) swap(a,b);
    int l = len[b - a + 1];
    return min_dep(md[a][l],md[b - (1 << l) + 1][l]);
}
int dis(int a,int b) {
    return dep[a] + dep[b] - 2 * dep[lca(a,b)] + 1;
}
void dfs1(int u) {
    dep[u] = dep[fa[u]] + 1;
    vis[u] = 1;
    pos[u] = ++bud;md[bud][0] = u;
    siz[u] = 1;
    for(int i = head[u] ; i ; i = E[i].next) {
	int v = E[i].to;
	if(!vis[v]) {
	    fa[v] = u;
	    dfs1(v);
	    md[++bud][0] = u;
	    worth[v] = E[i].val;
	    siz[u] += siz[v];
	    if(siz[v] > siz[son[u]]) son[u] = v;
	}
	else if(dep[v] < dep[u] && v != fa[u]) {
	    st = u;ed = v;ev = E[i].val;
	}
    }
}
void dfs2(int u) {
    vis[u] = 1;
    dfn[u] = ++idx;
    seq[idx] = u;
    if(!top[u]) top[u] = u;
    if(son[u]) {
	top[son[u]] = top[u];
	dfs2(son[u]);
    }
    for(int i = head[u] ; i ; i = E[i].next) {
	int v = E[i].to;
	if(!vis[v] && !(u == st && v == ed) && !(u == ed && v == st)) dfs2(v);
    }
}
namespace seg_tr {
    struct node {
	int L,R;
	int64 lmax,lmin,rmax,rmin,sum,val,mval;
	bool resign;
    }tr[MAXN * 4];
#define lc(u) u << 1
#define rc(u) u << 1 | 1
#define sum(u) tr[u].sum
#define lmin(u) tr[u].lmin
#define rmin(u) tr[u].rmin
#define lmax(u) tr[u].lmax
#define rmax(u) tr[u].rmax
#define val(u) tr[u].val
#define mval(u) tr[u].mval
    void resign(int u) {
	sum(u) = -sum(u);
	lmin(u) = -lmin(u);lmax(u) = -lmax(u);
	swap(lmin(u),lmax(u));
	rmin(u) = -rmin(u);rmax(u) = -rmax(u);
	swap(rmin(u),rmax(u));
	val(u) = -val(u);mval(u) = -mval(u);
	swap(val(u),mval(u));
	tr[u].resign ^= 1;
    }
    void push_down(int u) {
	if(tr[u].resign) {
	    resign(lc(u));resign(rc(u));
	    tr[u].resign = 0;
	}
    }
    void update(int u) {
	sum(u) = sum(lc(u)) + sum(rc(u));
	lmax(u) = max(lmax(lc(u)),sum(lc(u)) + lmax(rc(u)));
	lmin(u) = min(lmin(lc(u)),sum(lc(u)) + lmin(rc(u)));
	rmax(u) = max(rmax(rc(u)),rmax(lc(u)) + sum(rc(u)));
	rmin(u) = min(rmin(rc(u)),rmin(lc(u)) + sum(rc(u)));
	val(u) = max(val(lc(u)),val(rc(u)));
	val(u) = max(val(u),rmax(lc(u)) + lmax(rc(u)));
	mval(u) = min(mval(lc(u)),mval(rc(u)));
	mval(u) = min(mval(u),rmin(lc(u)) + lmin(rc(u)));
    }
    void build(int u,int l,int r) {
	tr[u].L = l;tr[u].R = r;
	if(l == r) {
	    lmax(u) = rmax(u) = val(u) = max(0LL,worth[seq[l]]);
	    lmin(u) = rmin(u) = mval(u) = min(0LL,worth[seq[l]]);
	    sum(u) = worth[seq[l]];
	    return;
	}
	int mid = (l + r) >> 1;
	build(u << 1,l,mid);
	build(u << 1 | 1,mid + 1,r);
	update(u);
    }
    int64 query_val(int u,int l,int r,int64 &v,bool on) {
	if(tr[u].L == l && tr[u].R == r) {
	    int64 res = val(u);
	    if(on) res = max(res,v + rmax(u));
	    else res = max(res,v + lmax(u));
	    if(on) v = max(v + sum(u),lmax(u));
	    else v = max(v + sum(u),rmax(u));
	    return res;
	}
	push_down(u);
	int mid = (tr[u].L + tr[u].R) >> 1;
	if(r <= mid) return query_val(lc(u),l,r,v,on);
	else if(l > mid) return query_val(rc(u),l,r,v,on);
	else {
	    if(!on) {
		int t = query_val(lc(u),l,mid,v,on);
		int h = query_val(rc(u),mid + 1,r,v,on);
		return max(t,h);
	    }
	    else {
		int t = query_val(rc(u),mid + 1,r,v,on);
		int h = query_val(lc(u),l,mid,v,on);
		return max(t,h);
	    }
	}
    }
    pair<int64,int64> query_max(int u,int l,int r,bool on) {
	if(tr[u].L == l && tr[u].R == r) {
	    if(!on) return mp(lmax(u),sum(u));
	    else return mp(rmax(u),sum(u));
	}
	push_down(u);
	int mid = (tr[u].L + tr[u].R) >> 1;
	if(r <= mid) return query_max(lc(u),l,r,on);
	else if(l > mid) return query_max(rc(u),l,r,on);
	else {
	    pair<int64,int64> t,h;
	    t = query_max(lc(u),l,mid,on);
	    h = query_max(rc(u),mid + 1,r,on);
	    if(!on) {
		return mp(max(t.fi,h.fi + t.se),t.se + h.se);
	    }
	    else {
		return mp(max(h.fi,t.fi + h.se),t.se + h.se);
	    }
	}
    }
    void change(int u,int l,int r) {
	if(tr[u].L == l && tr[u].R == r) {
	    resign(u);return;
	}
	push_down(u);
	int mid = (tr[u].L + tr[u].R) >> 1;
	if(r <= mid) change(lc(u),l,r);
	else if(l > mid) change(rc(u),l,r);
	else {
	    change(lc(u),l,mid);change(rc(u),mid + 1,r);
	}
	update(u);
    }
    int64 Query_max(int l,int r,int64 v,bool on) {
	pair<int64,int64> t = query_max(1,l,r,on);
	return max(t.fi,v + t.se);
    }
    void Change(int l,int r) {
	change(1,l,r);
    }
    int64 Query_val(int l,int r,int64 v,bool on) {
	return query_val(1,l,r,v,on);
    } 
}
void Init() {
    read(N);
    int u,v;int64 c;
    for(int i = 1 ; i <= N ; ++i) {
	read(u);read(v);read(c);
	add(u,v,c);add(v,u,c);
    }
    dfs1(1);
    memset(vis,0,sizeof(vis));
    dfs2(1);
    seg_tr::build(1,1,N);
    for(int j = 1 ; j <= 18 ; ++j) {
	for(int i = 1 ; i <= bud ; ++i) {
	    if(i + (1 << j) - 1 > bud) break;
	    md[i][j] = min_dep(md[i][j - 1],md[i + (1 << j - 1)][j - 1]);
	}
    }
    for(int i = 2 ; i <= bud ; ++i) len[i] = len[i / 2] + 1;
}
void Change_Path(int u,int v) {
    while(top[u] != top[v]) {
	if(dfn[top[u]] < dfn[top[v]]) swap(u,v);
	seg_tr::Change(dfn[top[u]],dfn[u]);
	u = fa[top[u]];
    }
    if(dep[u] > dep[v]) swap(u,v);
    if(u != v) seg_tr::Change(dfn[u] + 1,dfn[v]);
}
vector<pii> b;
pair<int64,int64> Query_Path(int u,int v,int64 pre) {
    int f = lca(u,v);
    int64 ans = pre;
    b.clear();
    while(top[u] != top[f]) {
	ans = max(ans,seg_tr::Query_val(dfn[top[u]],dfn[u],pre,1));
	pre = seg_tr::Query_max(dfn[top[u]],dfn[u],pre,0);
	u = fa[top[u]];
    }
    if(f != u) {
	ans = max(ans,seg_tr::Query_val(dfn[f] + 1,dfn[u],pre,1));
	pre = seg_tr::Query_max(dfn[f] + 1,dfn[u],pre,0);
    }
    while(top[v] != top[f]) {
	b.pb(mp(top[v],v));
	v = fa[top[v]];
    }
    if(f != v) {
	b.pb(mp(seq[dfn[f] + 1],v));
    }
    for(int i = b.size() - 1 ; i >= 0 ; --i) {
	ans = max(ans,seg_tr::Query_val(dfn[b[i].fi],dfn[b[i].se],pre,0));
	pre = seg_tr::Query_max(dfn[b[i].fi],dfn[b[i].se],pre,1);
    }
    return mp(ans,pre);
}
void Pf(int u,int v) {
    if(dis(u,st) + dis(ed,v) > dis(u,v)) Change_Path(u,v);
    else {Change_Path(u,st);Change_Path(ed,v);ev = -ev;}
}
int64 P(int u,int v) {
    if(dis(u,st) + dis(ed,v) > dis(u,v)) return Query_Path(u,v,0).fi;
    else {
	pair<int64,int64> t = Query_Path(u,st,0);
	t.se = max(0LL,t.se + ev);
	t.fi = max(t.fi,Query_Path(ed,v,t.se).fi);
	return t.fi;
    }
}
void Solve() {
    read(M);
    char op[5];
    int u,v;
    for(int i = 1 ; i <= M ; ++i) {
	scanf("%s",op + 1);read(u);read(v);
	if(dis(st,u) + dis(ed,v) > dis(st,v) + dis(ed,u)) swap(u,v);
	if(op[1] == 'f') Pf(u,v);
	else {out(P(u,v));enter;}
    }
}
int main() {
// #ifdef ivorysi
//     freopen("f1.in","r",stdin);
// #endif
    Init();
    Solve();
    return 0;
}
