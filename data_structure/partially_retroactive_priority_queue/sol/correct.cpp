#include <cstdio>
#include <algorithm>
#include <vector>

// Reference: Erik D. Demaine, John Iacono, and Stefan Langerman. Retroactive Data Structures.
struct partially_retroactive_priority_queue {
    struct seg_node { int sum = 0, pfxmin = 0, sfxmax = 0, minadd = 0, maxdel = -1; };

    std::vector<int> types, vals;
    std::vector<seg_node> segtree;
    long long sum_multiset = 0;
    int count_multiset = 0;
    int len = 0, seglen = 1;

    partially_retroactive_priority_queue(int n) {
        len = n + 1;
        types.assign(len, 2);
        vals.assign(len, -1);
        while (seglen < len) seglen *= 2;
        segtree.resize(seglen * 2);
    }

    bool comp(int i, int j) const {
        if (i == 0) return false;
        if (j == 0) return true;
        return vals[i] < vals[j];
    }

    void update(int i) {
        for (int j = (seglen + i) >> 1; j; j >>= 1) {
            seg_node &p = segtree[j], &l = segtree[j * 2], &r = segtree[j * 2 + 1];
            p.sum = l.sum + r.sum;
            p.pfxmin = std::min(l.pfxmin, l.sum + r.pfxmin);
            p.sfxmax = std::max(l.sfxmax + r.sum, r.sfxmax);
            if (comp(l.minadd, r.minadd)) p.minadd = l.minadd;
            else p.minadd = r.minadd;
            if (l.maxdel != -1 && (r.maxdel == -1 || comp(r.maxdel, l.maxdel))) p.maxdel = l.maxdel;
            else p.maxdel = r.maxdel;
        }
    }

    int next_bridge(int i) const {
        int acc = 0;
        for (int l = seglen, r = seglen + i + 1; l < r; l >>= 1, r >>= 1) {
            if (~r & 1) continue;
            r--;
            acc += segtree[r].sum;
        }
        if (acc == 0) return i;
        int l = seglen + i + 1;
        do {
            while (~l & 1) l >>= 1;
            if (acc + segtree[l].pfxmin == 0) {
                while (l < seglen) {
                    l <<= 1;
                    if (acc + segtree[l].pfxmin > 0) {
                        acc += segtree[l].sum;
                        l++;
                    }
                }
                return l - seglen;
            }
            acc += segtree[l].sum;
            l++;
        } while ((l & -l) != l);
        return len - 1;
    }

    int prev_bridge(int i) const {
        int acc = 0;
        for (int l = seglen + i, r = seglen * 2; l < r; l >>= 1, r >>= 1) {
            if (~l & 1) continue;
            acc += segtree[l].sum;
            l++;
        }
        if (acc == 0) return i;
        int r = seglen + i;
        do {
            r--;
            while (r > 1 && r & 1) r >>= 1;
            if (segtree[r].sfxmax + acc == 0) {
                while (r < seglen) {
                    r <<= 1;
                    r++;
                    if (segtree[r].sfxmax + acc < 0) {
                        acc += segtree[r].sum;
                        r--;
                    }
                }
                return r - seglen;
            }
            acc += segtree[r].sum;
        } while ((r & -r) != r);
        return 0;
    }

    int minadd(int i) const {
        int k = 0;
        int bridge = next_bridge(i);
        for (int l = seglen, r = seglen + bridge + 1; l < r; l >>= 1, r >>= 1) {
            if (l & 1) {
                if (comp(segtree[l].minadd, k)) k = segtree[l].minadd;
                l++;
            }
            if (r & 1) {
                r--;
                if (comp(segtree[r].minadd, k)) k = segtree[r].minadd;
            }
        }
        return k;
    }

    int maxdel(int i) const {
        int k = -1;
        int bridge = prev_bridge(i);
        for (int l = seglen + bridge, r = seglen * 2; l < r; l >>= 1, r >>= 1) {
            if (l & 1) {
                if (segtree[l].maxdel != -1 && (k == -1 || comp(k, segtree[l].maxdel))) k = segtree[l].maxdel;
                l++;
            }
            if (r & 1) {
                r--;
                if (segtree[r].maxdel != -1 && (k == -1 || comp(k, segtree[r].maxdel))) k = segtree[r].maxdel;
            }
        }
        return k;
    }

    void push(int i) {
        int k = maxdel(i);
        seg_node &n = segtree[seglen + i];
        if (k == -1 || comp(k, i)) {
            sum_multiset += vals[i];
            count_multiset++;
            n.minadd = i;
            update(i);
            return;
        }
        n.maxdel = i;
        n.sum = n.sfxmax = 1;
        update(i);
        seg_node &m = segtree[seglen + k];
        if (k == 0) {
            m.sum--;
            m.sfxmax--;
            if (m.sum == 0) m.maxdel = -1;
        } else {
            sum_multiset += vals[k];
            count_multiset++;
            m.minadd = k;
            m.maxdel = -1;
            m.sum = m.sfxmax = 0;
        }
        update(k);
    }

    void unpush(int i) {
        seg_node &n = segtree[seglen + i];
        if (n.sum == 0) {
            sum_multiset -= vals[i];
            count_multiset--;
            n.minadd = 0;
            update(i);
            return;
        }
        int k = minadd(i);
        n.maxdel = -1;
        n.sum = n.sfxmax = 0;
        update(i);
        seg_node &m = segtree[seglen + k];
        if (k > 0) {
            sum_multiset -= vals[k];
            count_multiset--;
        }
        m.minadd = 0;
        m.maxdel = k;
        m.sum++;
        m.sfxmax++;
        update(k);
    }

    void pop(int i) {
        int k = minadd(i);
        seg_node &n = segtree[seglen + i];
        n.sum = n.pfxmin = -1;
        update(i);
        if (k > 0) {
            sum_multiset -= vals[k];
            count_multiset--;
        }
        seg_node &m = segtree[seglen + k];
        m.minadd = 0;
        m.maxdel = k;
        m.sum++;
        m.sfxmax++;
        update(k);
    }

    void unpop(int i) {
        int k = maxdel(i);
        seg_node &n = segtree[seglen + i], &m = segtree[seglen + k];
        n.sum = n.pfxmin = 0;
        update(i);
        if (k == 0) {
            m.sum--;
            m.sfxmax--;
            if (m.sum == 0) m.maxdel = -1;
        } else {
            sum_multiset += vals[k];
            count_multiset++;
            m.minadd = k;
            m.maxdel = -1;
            m.sum = m.sfxmax = 0;
        }
        update(k);
    }

    void set(int i, int t, int x) {
        i++;
        if (types[i] == 0) unpush(i);
        else if (types[i] == 1) unpop(i);
        types[i] = t;
        vals[i] = x;
        if (t == 0) push(i);
        else if (t == 1) pop(i);
    }
};

int main() {
    int N, Q;
    scanf("%d%d", &N, &Q);
    partially_retroactive_priority_queue M(N);
    for (int i = 0; i < N; i++) {
        int t, x;
        scanf("%d%d", &t, &x);
        M.set(i, t, x);
    }
    while (Q--) {
        int k, a, b;
        scanf("%d%d%d", &k, &a, &b);
        M.set(k, a, b);
        printf("%d %lld\n", M.count_multiset, M.sum_multiset);
    }
}
