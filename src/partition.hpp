#ifndef PARTITION_HPP
#define PARTITION_HPP

#include "./bucket_list.hpp"

#include <string>
#include <unordered_map>
#include <unordered_set>

class Partition {
 public:
  Partition(
      const std::string& name, int num_cells, int num_nets, int num_pins,
      const std::unordered_set<int>& cell_ids,
      const std::unordered_map<int, std::vector<int>>& net_ids_from_cell_id);

  const std::unordered_set<int>& CellIds() const;
  int NumNetCells(int net_id) const;
  bool HasCell(int cell_id) const;
  int MaxGain() const;
  int MaxGainCellId() const;
  bool AreAllCellsLocked() const;
  const std::vector<int>& CellGains() const;

  void AddCell(int cell_id, const std::vector<int>& net_ids, int gain,
               bool is_locked);
  void RemoveCell(int cell_id, const std::vector<int>& net_ids, int gain,
                  bool is_locked);
  void InitializeBucketList(const std::vector<int>& gains);
  void UpdateBucketList(int cell_id, int old_gain, int new_gain,
                        bool is_locked);

 private:
  std::string name_;

  std::unordered_set<int> cell_ids_;
  std::vector<int> num_net_cells_from_net_id_;
  BucketList bucket_list_;
};

#endif
