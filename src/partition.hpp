#ifndef PARTITION_HPP
#define PARTITION_HPP

#include <string>
#include <unordered_set>
#include <vector>

class Partition {
 public:
  Partition(const std::string& name, int num_nets);

  int NumCells() const;
  int NumNetCells(int net_id) const;

  void AddCellId(int cell_id, const std::vector<int>& net_ids);
  void RemoveCellId(int cell_id, const std::vector<int>& net_ids);

 private:
  std::string name_;
  std::unordered_set<int> cell_ids_;
  std::vector<int> num_net_cells_from_id_;
};

#endif
