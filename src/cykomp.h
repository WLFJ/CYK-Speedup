#ifndef __CYK_OMP_H__
#define __CYK_OMP_H__

#include <array>
#include <assert.h>
#include <iostream>
#include <omp.h>
#include <string>
#include <unordered_map>
#include <vector>

namespace cyk {
using namespace std;
#define MAXN 1000

void load_data();
void init();

void calc();
void print_res();
}; // namespace cyk

#endif
