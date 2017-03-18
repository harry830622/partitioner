#ifndef PARTITIONER_HPP
#define PARTITIONER_HPP

#include "./cell.hpp"
#include "./net.hpp"
#include "./partition.hpp"
#include "./simple_parser.hpp"

#include <unordered_map>

class Partitioner {
 public:
  Partitioner(std::istream& input);

  void PartitionCells();

  void Output(std::ostream& output);

 private:
  int ComputeNumPins() const;
  bool ArePartitionsBalancedAfterMove(const Partition& from,
                                      const Partition& to) const;

  void Parse(std::istream& input);
  void InitializePartitions();
  void InitializeBucketLists();

  double balance_factor_;
  std::vector<Partition> partitions_;
  std::vector<Cell> cells_;
  std::vector<Net> nets_;
  std::unordered_map<std::string, int> cell_id_from_name_;
  std::unordered_map<std::string, int> net_id_from_name_;

  std::vector<Partition> best_partitions_;
};

#endif
