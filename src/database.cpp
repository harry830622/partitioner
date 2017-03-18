#include "./database.hpp"
#include "./simple_parser.hpp"

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

const Cell& Database::CellFromId(int cell_id) const {
  return cells_.at(cell_id);
}

const Net& Database::NetFromId(int net_id) const { return nets_.at(net_id); }

Cell& Database::CellFromId(int cell_id) { return cells_.at(cell_id); }

Net& Database::NetFromId(int net_id) { return nets_.at(net_id); }

void Database::Parse(istream& input) {
  simple_parser::Parser parser(input, ";");

  bool is_balance_factor_set = false;
  parser.Parse([&](const simple_parser::Tokens& tokens) -> bool {
    if (!tokens.empty()) {
      const string keyword(tokens[0]);
      if (!is_balance_factor_set) {
        balance_factor_ = stod(keyword);
        is_balance_factor_set = true;
      }

      if (keyword == "NET") {
        const string net_name(tokens[1]);
        nets_.push_back(Net(net_name));

        const int net_id = nets_.size() - 1;
        net_id_from_net_name_.insert({net_name, net_id});

        for (int i = 2; i < tokens.size(); ++i) {
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

          NetFromId(net_id).ConnectCell(cell_id);
          CellFromId(cell_id).ConnectNet(net_id);
        }
      }
    }

    return true;
  });
}
