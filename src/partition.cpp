#include "./partition.hpp"

#include <iostream>

using namespace std;

Partition::Partition(
    const string& name, int num_cells, int num_nets, int num_pins,
    const unordered_set<int>& cell_ids,
    const unordered_map<int, vector<int>>& net_ids_from_cell_id)
    : name_(name),
      cell_ids_(cell_ids),
      num_net_cells_from_net_id_(num_nets, 0),
      cell_gains_(num_cells, 0),
      bucket_list_(num_cells, num_pins) {
  for (int cell_id : cell_ids) {
    for (int net_id : net_ids_from_cell_id.at(cell_id)) {
      ++num_net_cells_from_net_id_.at(net_id);
    }
  }
}

const unordered_set<int>& Partition::CellIds() const { return cell_ids_; }

int Partition::NumNetCells(int net_id) const {
  return num_net_cells_from_net_id_.at(net_id);
}

bool Partition::HasCell(int cell_id) const {
  return cell_ids_.count(cell_id) == 1;
}

int Partition::MaxGain() const { return bucket_list_.MaxGain(); }

int Partition::MaxGainCellId() const { return bucket_list_.MaxGainCellId(); }

bool Partition::AreAllCellsLocked() const {
  return bucket_list_.AreAllCellsLocked();
}

const vector<int>& Partition::CellGains() const { return cell_gains_; }

void Partition::AddCell(int cell_id, const vector<int>& net_ids, int gain,
                        bool is_locked) {
  cell_ids_.insert(cell_id);
  for (int net_id : net_ids) {
    ++num_net_cells_from_net_id_.at(net_id);
  }

  cell_gains_.at(cell_id) = gain;

  bucket_list_.InsertCell(cell_id, gain, is_locked);
}

void Partition::RemoveCell(int cell_id, const vector<int>& net_ids, int gain,
                           bool is_locked) {
  cell_ids_.erase(cell_id);
  for (int net_id : net_ids) {
    --num_net_cells_from_net_id_.at(net_id);
  }

  cell_gains_.at(cell_id) = gain;

  bucket_list_.RemoveCell(cell_id, gain, is_locked);
}

void Partition::InitializeBucketList(const vector<int>& gains) {
  for (int i = 0; i < gains.size(); ++i) {
    if (cell_ids_.count(i) == 1) {
      bucket_list_.InsertCell(i, gains[i], false);
    }
  }
}

void Partition::UpdateBucketList(int cell_id, int old_gain, int new_gain,
                                 bool is_locked) {
  bucket_list_.RemoveCell(cell_id, old_gain, is_locked);
  bucket_list_.InsertCell(cell_id, new_gain, is_locked);
}
