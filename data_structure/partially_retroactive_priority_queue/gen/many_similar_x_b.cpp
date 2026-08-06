#include <cstdio>
#include <vector>
#include "random.h"
#include "../params.h"

int main(int, char* argv[]) {
    long long seed = atoll(argv[1]) ^ 0xb3074e5af32fb5c3;
    auto gen = Random(seed);

    int N = gen.uniform<int>(std::max<int>(N_MIN, N_MAX - 1000), N_MAX);
    int Q = gen.uniform<int>(std::max<int>(Q_MIN, Q_MAX - 1000), Q_MAX);
    printf("%d %d\n", N, Q);

    std::vector<int> vals;
    int num_vars = atoll(argv[1]) + 1;
    for (int i = 0; i < num_vars; i++) vals.push_back(gen.uniform<int>(X_MIN, X_MAX));

    auto gen_pair = [&]() -> std::pair<int, int> {
        double d = gen.uniform01();
        if (d < .1) return {2, -1};
        if (d < .4) return {1, -1};
        int x = vals[gen.uniform<int>(0, num_vars - 1)];
        return {0, x};
    };

    for (int i = 0; i < N; i++) {
        auto [t, x] = gen_pair();
        printf("%d %d\n", t, x);
    }

    for (int i = 0; i < Q; i++) {
        int k = gen.uniform<int>(0, N - 1);
        auto [a, b] = gen_pair();
        printf("%d %d %d\n", k, a, b);
    }
}
