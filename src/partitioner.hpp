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

  const Cell& CellById(int id) const;
  const Net& NetById(int id) const;

  void PartitionCells();

  void Output(std::ostream& output);

 private:
  void Parse(std::istream& input);
  void InitializePartitions();
  void InitializeBucketLists();
  void ComputeCellGains();

  double balance_factor_;
  Partition left_partition_;
  Partition right_partition_;
  std::vector<Cell> cells_;
  std::vector<Net> nets_;
  std::unordered_map<std::string, int> cell_id_from_name_;
  std::unordered_map<std::string, int> net_id_from_name_;
};

#endif
