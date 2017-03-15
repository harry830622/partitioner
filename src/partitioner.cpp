#include "./partitioner.hpp"

using namespace std;

Partitioner::Partitioner(istream& input)
    : balance_factor_(0.0),
      left_partition_("USELESS", 0, 0, 0),
      right_partition_("USELESS", 0, 0, 0) {
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
  int num_pins = 0;
  for (const Net& net : nets_) {
    num_pins += net.CellIds().size();
  }

  left_partition_ = Partition("LEFT", cells_.size(), nets_.size(), num_pins);
  right_partition_ = Partition("RIGHT", cells_.size(), nets_.size(), num_pins);

  int num_cells = cells_.size();
  for (int i = 0; i < num_cells; ++i) {
    const Cell& cell = cells_.at(i);
    if (i < num_cells / 2) {
      left_partition_.AddCell(i, cell.NetIds());
    } else {
      right_partition_.AddCell(i, cell.NetIds());
    }
  }
}

void Partitioner::InitializeBucketLists() {
  InitializeCellGains();

  vector<int> gains;
  for (const Cell& cell : cells_) {
    gains.push_back(cell.Gain());
  }
  left_partition_.InitializeBucketList(gains);
  right_partition_.InitializeBucketList(gains);
}

void Partitioner::InitializeCellGains() {
  for (int i = 0; i < cells_.size(); ++i) {
    Cell& cell = cells_[i];
    for (int net_id : cell.NetIds()) {
      int num_from_net_cells;
      int num_to_net_cells;
      if (left_partition_.HasCell(i)) {
        num_from_net_cells = left_partition_.NumNetCells(net_id);
        num_to_net_cells = right_partition_.NumNetCells(net_id);
      } else {
        num_from_net_cells = right_partition_.NumNetCells(net_id);
        num_to_net_cells = left_partition_.NumNetCells(net_id);
      }

      if (num_from_net_cells == 1) {
        cell.IncrementGain();
      } else if (num_to_net_cells == 0) {
        cell.DecrementGain();
      }
    }
  }
}
