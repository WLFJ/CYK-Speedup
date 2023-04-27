#include "cykomp.h"
#include <benchmark/benchmark.h>
#include <iostream>

using namespace std;

static void BM_cyk(benchmark::State &state) {
  const char *path = "test/input2.txt";

  FILE *file = freopen(path, "r", stdin); // 将文件重定向到标准输入

  if (file == nullptr) {
    std::cerr << "Failed to open file " << path << std::endl;
    return;
  }

  cyk::load_data();

  for (auto _ : state) {
    state.PauseTiming();
    cyk::init();
    state.ResumeTiming();
    cyk::calc();
  }

  // cyk::print_res();
}

BENCHMARK(BM_cyk);

BENCHMARK_MAIN();
