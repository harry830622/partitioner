#ifndef PARTITIONER_HPP
#define PARTITIONER_HPP

#include "./database.hpp"
#include "./bucket_list.hpp"

class Partitioner {
 public:
  Partitioner(const Database& database);

  void PartitionCells();

 private:
  std::vector<BucketList> partitions_;
};

#endif
