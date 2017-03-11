#include "./partition.hpp"

using namespace std;

Partition::Partition(const string& name, int num_nets)
    : name_(name), num_net_cells_from_id_(num_nets, 0) {}

int Partition::NumCells() const { return cell_ids_.size(); }

int Partition::NumNetCells(int net_id) const {
  return num_net_cells_from_id_.at(net_id);
}

void Partition::AddCellId(int cell_id, const vector<int>& net_ids) {
  cell_ids_.insert(cell_id);
  for (int net_id : net_ids) {
    ++num_net_cells_from_id_.at(net_id);
  }
}

void Partition::RemoveCellId(int cell_id, const vector<int>& net_ids) {
  cell_ids_.erase(cell_id);
  for (int net_id : net_ids) {
    --num_net_cells_from_id_.at(net_id);
  }
}
