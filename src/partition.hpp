#ifndef PARTITION_HPP
#define PARTITION_HPP

#include "./bucket_list.hpp"

#include <string>
#include <unordered_set>

class Partition {
 public:
  Partition(const std::string& name, int num_nets, int num_pins);

  int NumCells() const;
  int NumNetCells(int net_id) const;

  void AddCellId(int cell_id, const std::vector<int>& net_ids);
  void RemoveCellId(int cell_id, const std::vector<int>& net_ids);
  void InitializeBucketList(const std::vector<int>& gains);
  void UpdateBucketList(const std::vector<int>& old_gains,
                        const std::vector<int>& new_gains);

 private:
  std::string name_;

  std::unordered_set<int> cell_ids_;
  std::vector<int> num_net_cells_from_id_;
  BucketList bucket_list_;
};

#endif
