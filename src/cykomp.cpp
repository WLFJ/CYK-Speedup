#include "cykomp.h"
namespace cyk {

// pair<root,num>
unordered_map<unsigned, unsigned> dp[MAXN][MAXN];
vector<int> role[MAXN][MAXN];
unordered_map<char, vector<int>> mm;

int num_v, len, role_n, a, b, c;
string str;
char cc;

inline void merge(unordered_map<unsigned, unsigned> &mem,
                  const unordered_map<unsigned, unsigned> &va,
                  const unordered_map<unsigned, unsigned> &vb);

void calc() {
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
}

void merge(unordered_map<unsigned, unsigned> &mem,
           const unordered_map<unsigned, unsigned> &va,
           const unordered_map<unsigned, unsigned> &vb) {
  for (auto ita = va.begin(); ita != va.end(); ++ita) {
    for (auto itb = vb.begin(); itb != vb.end(); ++itb) {
      auto &vroot = role[ita->first][itb->first];
      for (auto root : vroot) {
#ifdef DEBUG
        cout << "MERGE" << " " << ita->first << " " << itb->second << " into " << root << endl;
#endif
        mem[root] += ita->second * itb->second;
      }
    }
  }
  return;
}

void init() {
  for (int i = 0; i < len; i++)
    for (int j = 0; j < len; j++)
      dp[i][j].clear();
  for (int i = 0; i < len; i++) {
    // init dp[0][i]
    char t = str[i];
    dp[0][i].clear();
    for (auto it = mm[t].begin(); it != mm[t].end(); ++it) {
      dp[0][i][*it] = 1;
    }
  }
}

void load_data() {
  std::ignore = scanf("%d\n", &num_v);
  std::ignore = scanf("%d\n", &role_n);
  for (int i = 0; i < MAXN; i++)
    for (int j = 0; j < MAXN; j++)
      role[i][j].clear();
  for (int i = 0; i < role_n; i++) {
    std::ignore = scanf("<%d>::=<%d><%d>\n", &a, &b, &c);
    role[b][c].push_back(a);
  }

  std::ignore = scanf("%d\n", &role_n);
  for (int i = 0; i < role_n; i++) {
    std::ignore = scanf("<%d>::=%c\n", &a, &cc);
    mm[cc].push_back(a);
  }

  cin >> len;
  cin >> str;
}

void print_res() {
  auto end_layer = dp[len - 1][0];
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
  return;
}
} // namespace cyk
