#include "./partitioner.hpp"

#include <iostream>

using namespace std;

Partitioner::Partitioner(istream& input) : balance_factor_(0.0) {
  Parse(input);
  InitializePartitions();
  InitializeBucketLists();
  best_partitions_ = partitions_;
}

void Partitioner::PartitionCells() {
  const int num_cells = cells_.size();
  const int num_pins = ComputeNumPins();

  int max_partial_sum;
  do {
    Partition& left_partition = best_partitions_[0];
    Partition& right_partition = best_partitions_[1];

    vector<vector<Partition>> partitions_snapshots;
    vector<int> partial_sums;
    max_partial_sum = -1 * num_pins - 1;

    for (int i = 0; i < num_cells - 1; ++i) {
      bool is_cell_from_left_partition = true;
      int cell_id = -1;

      if (left_partition.MaxGain() > right_partition.MaxGain() &&
          ArePartitionsBalancedAfterMove(left_partition, right_partition)) {
        cell_id = left_partition.MaxGainCellId();
      }

      if (cell_id == -1) {
        is_cell_from_left_partition = false;
        cell_id = right_partition.MaxGainCellId();
      }

      Partition& from_partition =
          is_cell_from_left_partition ? left_partition : right_partition;
      Partition& to_partition =
          is_cell_from_left_partition ? right_partition : left_partition;

      Cell& cell = cells_.at(cell_id);
      int old_cell_gain = cell.Gain();

      for (int net_id : cell.NetIds()) {
        const Net& net = nets_.at(net_id);

        int num_from_partition_net_cells = from_partition.NumNetCells(net_id);
        int num_to_partition_net_cells = to_partition.NumNetCells(net_id);

        if (num_from_partition_net_cells == 1) {
          cell.DecrementGain();
        } else if (num_to_partition_net_cells == 0) {
          for (int net_cell_id : net.CellIds()) {
            Cell& net_cell = cells_.at(net_cell_id);

            int old_net_cell_gain = net_cell.Gain();
            net_cell.IncrementGain();

            if (left_partition.HasCell(net_cell_id)) {
              left_partition.UpdateBucketList(net_cell_id, old_net_cell_gain,
                                              net_cell.Gain(),
                                              net_cell.IsLocked());
            } else {
              right_partition.UpdateBucketList(net_cell_id, old_net_cell_gain,
                                               net_cell.Gain(),
                                               net_cell.IsLocked());
            }
          }
        } else if (num_to_partition_net_cells == 1) {
          for (int net_cell_id : net.CellIds()) {
            if (to_partition.HasCell(net_cell_id)) {
              Cell& net_cell = cells_.at(net_cell_id);

              int old_net_cell_gain = net_cell.Gain();
              net_cell.DecrementGain();

              to_partition.UpdateBucketList(net_cell_id, old_net_cell_gain,
                                            net_cell.Gain(),
                                            net_cell.IsLocked());
              break;
            }
          }
        }
      }

      cell.Print();
      cout << old_cell_gain << endl;

      from_partition.RemoveCell(cell_id, cell.NetIds(), old_cell_gain, false);
      cell.Lock();
      from_partition.AddCell(cell_id, cell.NetIds(), cell.Gain(), true);

      cell.Print();

      for (int net_id : cell.NetIds()) {
        const Net& net = nets_.at(net_id);

        int num_from_partition_net_cells = from_partition.NumNetCells(net_id);
        int num_to_partition_net_cells = to_partition.NumNetCells(net_id);

        if (num_to_partition_net_cells == 1) {
          cell.IncrementGain();
        } else if (num_from_partition_net_cells == 0) {
          for (int net_cell_id : net.CellIds()) {
            Cell& net_cell = cells_.at(net_cell_id);

            int old_net_cell_gain = net_cell.Gain();
            net_cell.DecrementGain();

            if (left_partition.HasCell(net_cell_id)) {
              left_partition.UpdateBucketList(net_cell_id, old_net_cell_gain,
                                              net_cell.Gain(),
                                              net_cell.IsLocked());
            } else {
              right_partition.UpdateBucketList(net_cell_id, old_net_cell_gain,
                                               net_cell.Gain(),
                                               net_cell.IsLocked());
            }
          }
        } else if (num_from_partition_net_cells == 1) {
          for (int net_cell_id : net.CellIds()) {
            if (from_partition.HasCell(net_cell_id)) {
              Cell& net_cell = cells_.at(net_cell_id);

              int old_net_cell_gain = net_cell.Gain();
              net_cell.IncrementGain();

              to_partition.UpdateBucketList(net_cell_id, old_net_cell_gain,
                                            net_cell.Gain(),
                                            net_cell.IsLocked());
              break;
            }
          }
        }
      }

      if (partial_sums.empty()) {
        partial_sums.push_back(old_cell_gain);
      } else {
        partial_sums.push_back(partial_sums.back() + old_cell_gain);
      }

      partitions_snapshots.push_back(partitions_);

      for (Cell& cell : cells_) {
        cell.Unlock();
      }
    }

    for (int i = 0; i < partial_sums.size(); ++i) {
      if (partial_sums[i] > max_partial_sum) {
        max_partial_sum = partial_sums[i];
        best_partitions_ = partitions_snapshots.at(i);
      }
    }
  } while (max_partial_sum > 0);
}

int Partitioner::ComputeNumPins() const {
  int num_pins = 0;
  for (const Net& net : nets_) {
    num_pins += net.CellIds().size();
  }

  return num_pins;
}

bool Partitioner::ArePartitionsBalancedAfterMove(const Partition& from,
                                                 const Partition& to) const {
  if (from.CellIds().size() - 1 <= (1 - balance_factor_) / 2 ||
      to.CellIds().size() + 1 >= (1 + balance_factor_) / 2) {
    return false;
  }

  return true;
}

void Partitioner::Parse(istream& input) {
  simple_parser::Parser parser(input, ";");

  bool is_balance_factor_set = false;
  parser.Parse([&](const simple_parser::Tokens& tokens) -> bool {
    if (!tokens.empty()) {
      string keyword = tokens[0];
      if (!is_balance_factor_set) {
        balance_factor_ = stod(keyword);
        is_balance_factor_set = true;
      }

      if (keyword == "NET") {
        string net_name(tokens[1]);
        int net_id = nets_.size();
        nets_.push_back(Net(net_name));
        net_id_from_name_.insert({net_name, net_id});

        for (int i = 2; i < tokens.size(); ++i) {
          string cell_name = tokens[i];
          int cell_id = -1;
          if (cell_id_from_name_.count(cell_name) == 1) {
            cell_id = cell_id_from_name_.at(cell_name);
          } else {
            cell_id = cells_.size();
            cells_.push_back(Cell(cell_name));
            cell_id_from_name_.insert({cell_name, cell_id});
          }
          nets_.at(net_id).ConnectCell(cell_id);
          cells_.at(cell_id).ConnectNet(net_id);
        }
      }
    }

    return true;
  });
}

void Partitioner::InitializePartitions() {
  const int num_cells = cells_.size();
  const int num_nets = nets_.size();
  const int num_pins = ComputeNumPins();

  unordered_set<int> left_partition_cell_ids;
  unordered_map<int, vector<int>> left_partition_net_ids_from_cell_id;
  for (int i = 0; i < num_cells / 2; ++i) {
    const Cell& cell = cells_.at(i);

    left_partition_cell_ids.insert(i);
    left_partition_net_ids_from_cell_id.insert({i, cell.NetIds()});
  }
  partitions_.push_back(Partition("LEFT", num_cells, num_nets, num_pins,
                                  left_partition_cell_ids,
                                  left_partition_net_ids_from_cell_id));

  unordered_set<int> right_partition_cell_ids;
  unordered_map<int, vector<int>> right_partition_net_ids_from_cell_id;
  for (int i = num_cells / 2; i < num_cells; ++i) {
    const Cell& cell = cells_.at(i);

    right_partition_cell_ids.insert(i);
    right_partition_net_ids_from_cell_id.insert({i, cell.NetIds()});
  }
  partitions_.push_back(Partition("RIGHT", num_cells, num_nets, num_pins,
                                  right_partition_cell_ids,
                                  right_partition_net_ids_from_cell_id));
}

void Partitioner::InitializeBucketLists() {
  Partition& left_partition = partitions_[0];
  Partition& right_partition = partitions_[1];

  for (int i = 0; i < cells_.size(); ++i) {
    Cell& cell = cells_[i];
    for (int net_id : cell.NetIds()) {
      int num_from_partition_net_cells =
          left_partition.HasCell(i) ? left_partition.NumNetCells(net_id)
                                    : right_partition.NumNetCells(net_id);
      int num_to_partition_net_cells = left_partition.HasCell(i)
                                           ? right_partition.NumNetCells(net_id)
                                           : left_partition.NumNetCells(net_id);

      if (num_from_partition_net_cells == 1) {
        cell.IncrementGain();
      } else if (num_to_partition_net_cells == 0) {
        cell.DecrementGain();
      }
    }
  }

  vector<int> gains;
  for (const Cell& cell : cells_) {
    gains.push_back(cell.Gain());
  }
  left_partition.InitializeBucketList(gains);
  right_partition.InitializeBucketList(gains);
}
