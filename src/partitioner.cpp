#include "./partitioner.hpp"

using namespace std;

Partitioner::Partitioner(istream& input) {
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
        nets_.push_back(Net(net_id, net_name));
        net_id_from_name_.insert({net_name, net_id});

        for (int i = 2; i < tokens.size(); ++i) {
          string cell_name = tokens[i];
          int cell_id = -1;
          if (cell_id_from_name_.count(cell_name) == 1) {
            cell_id = cell_id_from_name_.at(cell_name);
          } else {
            cell_id = cells_.size();
            cells_.push_back(Cell(cell_id, cell_name));
            cell_id_from_name_.insert({cell_name, cell_id});
          }
          nets_.at(net_id).AddCell(cell_id);
          cells_.at(cell_id).AddNet(net_id);
        }
      }
    }

    return true;
  });
}
