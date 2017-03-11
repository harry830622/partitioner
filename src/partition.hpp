#ifndef PARTITION_HPP
#define PARTITION_HPP

#include <string>
#include <unordered_map>
#include <unordered_set>

class Partition {
 public:
  Partition(const std::string& name);

  int NumCells() const;

  void AddCell();
  void RemoveCell();

 private:
  std::string name_;
  std::unordered_set<int> cell_ids_;
  std::unordered_map<int, int> num_net_cells_from_id_;
};

#endif
