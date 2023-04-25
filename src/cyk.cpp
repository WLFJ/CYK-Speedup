#include "cykomp.h"

int main(void) {
	cyk::load_data();

	cyk::init();

	cyk::calc();

	cyk::print_res();
  return 0;
}

