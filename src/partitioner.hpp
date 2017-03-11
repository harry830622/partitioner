#ifndef PARTITIONER_HPP
#define PARTITIONER_HPP

#include "./cell.hpp"
#include "./net.hpp"
#include "./simple_parser.hpp"

#include <unordered_map>

class Partitioner {
 public:
  Partitioner(std::istream& input);

 private:
  double balance_factor_;
  std::vector<Cell> cells_;
  std::vector<Net> nets_;
  std::unordered_map<std::string, int> cell_id_from_name_;
  std::unordered_map<std::string, int> net_id_from_name_;
};

#endif
