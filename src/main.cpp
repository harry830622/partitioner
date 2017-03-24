#include "./partitioner.hpp"

#include <fstream>

using namespace std;

int main(int argc, char* argv[]) {
  ifstream input_file(argv[1]);

  Database database(input_file);
  /* database.Print(); */

  Partitioner partitioner(database);
  partitioner.PartitionCells();

  ofstream output_file(argv[2]);
  partitioner.Output(output_file);

  return 0;
}
