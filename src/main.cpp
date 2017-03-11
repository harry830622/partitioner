#include "./partitioner.hpp"

#include <fstream>

using namespace std;

int main(int argc, char* argv[]) {
  ifstream input_file(argv[1]);

  Partitioner partitioner(input_file);

  return 0;
}
