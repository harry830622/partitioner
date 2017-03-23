#include "./partitioner.hpp"

using namespace std;

Partitioner::Partitioner(const Database& database)
    : partitions_(2, BucketList(database.NumCells(), database.NumNets(),
                                database.ComputeNumPins())) {
  const int num_all_cells = database.NumCells();
  const int initial_gain = 0;
  BucketList& left_partition = partitions_[0];
  BucketList& right_partition = partitions_[1];
  for (int i = 0; i < num_all_cells / 2; ++i) {
    const int cell_id = i;
    const Cell& cell = database.CellFromId(cell_id);
    left_partition.InsertCell(cell_id, cell.NetIds(), initial_gain, false);
  }
  for (int i = num_all_cells / 2; i < num_all_cells; ++i) {
    const int cell_id = i;
    const Cell& cell = database.CellFromId(cell_id);
    right_partition.InsertCell(cell_id, cell.NetIds(), initial_gain, false);
  }

  vector<int> gain_from_cell_id(num_all_cells, initial_gain);
  for (int i = 0; i < num_all_cells; ++i) {
    const int cell_id = i;
    const Cell& cell = database.CellFromId(cell_id);

    BucketList& from_partition =
        left_partition.HasCell(cell_id) ? left_partition : right_partition;
    BucketList& to_partition =
        left_partition.HasCell(cell_id) ? right_partition : left_partition;

    for (int net_id : cell.NetIds()) {
      const int num_from_partition_cells = from_partition.NumNetCells(net_id);
      const int num_to_partition_cells = to_partition.NumNetCells(net_id);
      if (num_from_partition_cells == 1) {
        ++gain_from_cell_id.at(cell_id);
      } else if (num_to_partition_cells == 0) {
        --gain_from_cell_id.at(cell_id);
      }
    }
  }

  for (int i = 0; i < gain_from_cell_id.size(); ++i) {
    const int cell_id = i;
    const int gain = gain_from_cell_id.at(cell_id);
    if (left_partition.HasCell(cell_id)) {
      left_partition.UpdateCell(cell_id, gain, false);
    } else {
      right_partition.UpdateCell(cell_id, gain, false);
    }
  }
}

void Partitioner::PartitionCells() {}
