#include <array>
#include <assert.h>
#include <iostream>
#include <omp.h>
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;

#define MAXN 1000

// pair<root,num>
unordered_map<unsigned, unsigned> dp[MAXN][MAXN];
vector<int> role[MAXN][MAXN];
unordered_map<char, vector<int>> mm;

inline void merge(unordered_map<unsigned, unsigned> &mem,
                  const unordered_map<unsigned, unsigned> &va,
                  const unordered_map<unsigned, unsigned> &vb) {
  for (auto ita = va.begin(); ita != va.end(); ++ita) {
    for (auto itb = vb.begin(); itb != vb.end(); ++itb) {
      auto &vroot = role[ita->first][itb->first];
      for (auto root : vroot) {
        // cout << "MERGE" << " " << ita->first << " " << itb->second << endl;
        mem[root] += ita->second * itb->second;
      }
    }
  }
  return;
}

int main(void) {
  int num_v, len, role_n, a, b, c;
  string str;
  char cc;

  scanf("%d\n", &num_v);
  scanf("%d\n", &role_n);
  for (int i = 0; i < role_n; i++) {
    scanf("<%d>::=<%d><%d>\n", &a, &b, &c);
    role[b][c].push_back(a);
  }

  scanf("%d\n", &role_n);
  for (int i = 0; i < role_n; i++) {
    scanf("<%d>::=%c\n", &a, &cc);
    mm[cc].push_back(a);
  }

  cin >> len;
  cin >> str;

  for (int i = 0; i < len; i++) {
    // init dp[0][i]
    char t = str[i];
    for (auto it = mm[t].begin(); it != mm[t].end(); ++it) {
      dp[0][i][*it] = 1;
    }
  }

  for (int i = 1; i < len; ++i) {
#ifdef OMP
#pragma omp parallel for
#endif
    for (int j = 0; j < len - i; ++j) { // parallel
      /*
       * 0,0 0,1 0,2 0,3 0,4 <- inited.
       * 1,0 1,1 1,2,1,3
       * 2,0 2,1 2,2
       * 3,0 3,1
       * 4,0
       */
      for (int k = 1; k <= i; ++k) {
        auto &va = dp[k - 1][j];
        auto &vb = dp[i - k][j + k];
        merge(dp[i][j], va, vb);
      }
    }
#ifdef DEBUG
    cout << "---> " << i << "/" << len << endl;
#endif
  }

  auto end_layer = std::move(dp[len - 1][0]);
  // cout << "root, count" << endl;
  for (auto it = end_layer.begin(); it != end_layer.end(); ++it) {
    if (it->first == 0) {
      cout << it->second << endl;
      // cout << it->first << " " << it->second << endl;
      goto end;
    }
  }
  cout << 0 << endl;
end:
  return 0;
}
