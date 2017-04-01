#include "./database.hpp"
#include "./simple_parser.hpp"

#include <algorithm>

using namespace std;

Database::Database(istream& input) { Parse(input); }

void Database::Print(ostream& os, int num_spaces) const {
  os << string(num_spaces, ' ') << "balance_factor_: " << balance_factor_
     << endl;
  os << string(num_spaces, ' ') << "cells_: " << endl;
  for (const Cell& cell : cells_) {
    cell.Print(os, num_spaces + 2);
  }
  os << string(num_spaces, ' ') << "nets_: " << endl;
  for (const Net& net : nets_) {
    net.Print(os, num_spaces + 2);
  }
}

double Database::BalanceFactor() const { return balance_factor_; }

int Database::NumCells() const { return cells_.size(); }

int Database::NumNets() const { return nets_.size(); }

int Database::ComputeNumPins() const {
  int num_pins = 0;
  for (const Net& net : nets_) {
    num_pins += net.NumCells();
  }

  return num_pins;
}

const Cell& Database::CellFromId(int cell_id) const {
  return cells_.at(cell_id);
}

const Net& Database::NetFromId(int net_id) const { return nets_.at(net_id); }

Cell& Database::CellFromId(int cell_id) { return cells_.at(cell_id); }

Net& Database::NetFromId(int net_id) { return nets_.at(net_id); }

void Database::Parse(istream& input) {
  simple_parser::Parser parser(input, ";");

  bool is_balance_factor_set = false;
  int net_id = -1;
  vector<int> net_cell_ids;
  parser.Parse([&](const simple_parser::Tokens& tokens) -> bool {
    if (!tokens.empty()) {
      const string keyword(tokens[0]);

      if (!is_balance_factor_set) {
        balance_factor_ = stod(keyword);
        is_balance_factor_set = true;

        return true;
      }

      int start = 0;
      if (keyword == "NET") {
        start = 2;
        net_cell_ids = {};
        const string net_name(tokens[1]);
        nets_.push_back(Net(net_name));
        net_id = nets_.size() - 1;
      }

      for (int i = start; i < tokens.size(); ++i) {
        const string cell_name(tokens[i]);
        const int cell_id = [&]() -> int {
          if (cell_id_from_cell_name_.count(cell_name) == 1) {
            return cell_id_from_cell_name_.at(cell_name);
          }

          cells_.push_back(Cell(cell_name));

          int cell_id = cells_.size() - 1;
          cell_id_from_cell_name_.insert({cell_name, cell_id});

          return cell_id;
        }();

        if (find(net_cell_ids.begin(), net_cell_ids.end(), cell_id) !=
            net_cell_ids.end()) {
          /* cout << NetFromId(net_id).Name() << " connects to duplicated cells"
           */
          /*      << endl; */
          continue;
        }
        net_cell_ids.push_back(cell_id);

        if (start == 2 && tokens.size() == 3) {
          /* cout << NetFromId(net_id).Name() << " connects to only a cell" */
          /*      << endl; */
          nets_.pop_back();
          break;
        }

        NetFromId(net_id).ConnectCell(cell_id);
        CellFromId(cell_id).ConnectNet(net_id);
      }
    }

    return true;
  });
}
