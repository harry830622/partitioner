#ifndef PARTITIONER_HPP
#define PARTITIONER_HPP

#include "./bucket_list.hpp"
#include "./database.hpp"

#include <map>

class Partitioner {
 public:
  Partitioner(Database& database);

  void PartitionCells();

 private:
  bool ArePartitionsBalancedAfterMove(const BucketList& from_partition,
                                      const BucketList& to_partition) const;
  bool IsNetLocked(int net_id) const;

  void InitializeGains();
  void UpdateGains(const std::vector<int>& gain_from_cell_id);
  void MoveCell(int cell_id);
  void UnlockAllCells();

  Database& database_;

  std::vector<BucketList> partitions_;
};

#endif
