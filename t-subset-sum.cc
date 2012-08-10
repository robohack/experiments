// Input:
// c (capacity of the knapsack)
// n (number of items)
// w_1 (weight of item 1)
// ...
// w_n (weight of item n)
//
// Output:
// z (optimal solution)
// n
// x_1 (indicator for item 1)
// ...
// x_n (indicator for item n)

#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

using namespace std;

int main() {
  int c = 0;
  cin >> c;
  int n = 0;
  cin >> n;
  assert(n > 0);
  vector<int> w(n);
  int sum_w = 0;
  int r = 0;
  for (int j = 0; j < n; ++j) {
    cin >> w[j];
    assert(w[j] > 0);
    sum_w += w[j];
    assert(w[j] <= c);
    r = max(r, w[j]);
  }
  assert(sum_w > c);
  int b;
  int w_bar = 0;
  for (b = 0; w_bar + w[b] <= c; ++b) {
    w_bar += w[b];
  }
  vector<vector<int> > s(n - b + 1, vector<int>(2 * r));
  vector<int>::iterator s_b_1 = s[0].begin() + (r - 1);
  for (int mu = -r + 1; mu <= 0; ++mu) {
    s_b_1[mu] = -1;
  }
  for (int mu = 1; mu <= r; ++mu) {
    s_b_1[mu] = 0;
  }
  s_b_1[w_bar - c] = b;
  for (int t = b; t < n; ++t) {
    vector<int>::const_iterator s_t_1 = s[t - b].begin() + (r - 1);
    vector<int>::iterator s_t = s[t - b + 1].begin() + (r - 1);
    for (int mu = -r + 1; mu <= r; ++mu) {
      s_t[mu] = s_t_1[mu];
    }
    for (int mu = -r + 1; mu <= 0; ++mu) {
      int mu_prime = mu + w[t];
      s_t[mu_prime] = max(s_t[mu_prime], s_t_1[mu]);
    }
    for (int mu = w[t]; mu >= 1; --mu) {
      for (int j = s_t[mu] - 1; j >= s_t_1[mu]; --j) {
        int mu_prime = mu - w[j];
        s_t[mu_prime] = max(s_t[mu_prime], j);
      }
    }
  }
  bool solved = false;
  int z;
  vector<int>::const_iterator s_n_1 = s[n - b].begin() + (r - 1);
  for (z = 0; z >= -r + 1; --z) {
    if (s_n_1[z] >= 0) {
      solved = true;
      break;
    }
  }
  if (solved) {
    cout << c + z << '\n' << n << '\n';
    vector<bool> x(n, false);
    for (int j = 0; j < b; ++j) x[j] = true;
    for (int t = n - 1; t >= b; --t) {
      vector<int>::const_iterator s_t = s[t - b + 1].begin() + (r - 1);
      vector<int>::const_iterator s_t_1 = s[t - b].begin() + (r - 1);
      while (true) {
        int j = s_t[z];
        assert(j >= 0);
        int z_unprime = z + w[j];
        if (z_unprime > r || j >= s_t[z_unprime]) break;
        z = z_unprime;
        x[j] = false;
      }
      int z_unprime = z - w[t];
      if (z_unprime >= -r + 1 && s_t_1[z_unprime] >= s_t[z]) {
        z = z_unprime;
        x[t] = true;
      }
    }
    for (int j = 0; j < n; ++j) {
      cout << x[j] << '\n';
    }
  }
}
