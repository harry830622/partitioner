#ifndef PARTITIONER_HPP
#define PARTITIONER_HPP

class Partitioner {
 public:
  Partitioner();

  void PartitionCells();

 private:
  void InitializePartitions();
};

#endif
