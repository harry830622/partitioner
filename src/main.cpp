#include "./database.hpp"

#include <fstream>

using namespace std;

int main(int argc, char* argv[]) {
  ifstream input_file(argv[1]);

  Database database(input_file);
  /* database.Print(); */

  return 0;
}
