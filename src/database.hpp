#ifndef DATABASE_HPP
#define DATABASE_HPP

#include "./cell.hpp"
#include "./net.hpp"

#include <unordered_map>

class Database {
 public:
  Database(std::istream& input);

  void Print(std::ostream& os = std::cout, int num_spaces = 0) const;

  double BalanceFactor() const;
  const Cell& CellFromId(int cell_id) const;
  const Net& NetFromId(int net_id) const;

  Cell& CellFromId(int cell_id);
  Net& NetFromId(int net_id);

 private:
  void Parse(std::istream& input);

  double balance_factor_;
  std::vector<Cell> cells_;
  std::vector<Net> nets_;

  std::unordered_map<std::string, int> cell_id_from_cell_name_;
  std::unordered_map<std::string, int> net_id_from_net_name_;
};

#endif
