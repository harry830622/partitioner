#include "./partitioner.hpp"

using namespace std;

Partitioner::Partitioner(istream& input) : balance_factor_(0.0) {
  Parse(input);
  InitializePartitions();
  InitializeBucketLists();
}

void Partitioner::PartitionCells() {}

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
  const int num_pins = [&]() {
    int result = 0;
    for (const Net& net : nets_) {
      result += net.CellIds().size();
    }

    return result;
  }();

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
      int num_from_net_cells;
      int num_to_net_cells;
      if (left_partition.HasCell(i)) {
        num_from_net_cells = left_partition.NumNetCells(net_id);
        num_to_net_cells = right_partition.NumNetCells(net_id);
      } else {
        num_from_net_cells = right_partition.NumNetCells(net_id);
        num_to_net_cells = left_partition.NumNetCells(net_id);
      }

      if (num_from_net_cells == 1) {
        cell.IncrementGain();
      } else if (num_to_net_cells == 0) {
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
