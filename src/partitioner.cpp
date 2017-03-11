#include "./partitioner.hpp"

using namespace std;

Partitioner::Partitioner(istream& input)
    : balance_factor_(0.0),
      left_partition_("USELESS", 0),
      right_partition_("USELESS", 0) {
  Parse(input);
  InitializePartitions();
  ComputeCellGains();
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
          nets_.at(net_id).AddCellId(cell_id);
          cells_.at(cell_id).AddNetId(net_id);
        }
      }
    }

    return true;
  });
}

void Partitioner::InitializePartitions() {
  left_partition_ = Partition("LEFT", nets_.size());
  right_partition_ = Partition("RIGHT", nets_.size());

  for (int i = 0; i < cells_.size() / 2; ++i) {
    const Cell& cell = cells_.at(i);
    left_partition_.AddCellId(i, cell.NetIds());
  }
  for (int i = cells_.size() / 2; i < cells_.size(); ++i) {
    const Cell& cell = cells_.at(i);
    right_partition_.AddCellId(i, cell.NetIds());
  }
}

void Partitioner::ComputeCellGains() {
  for (Cell& cell : cells_) {
    for (int net_id : cell.NetIds()) {
      int num_from_net_cells;
      int num_to_net_cells;
      if (cell.PartitionName() == "LEFT") {
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
