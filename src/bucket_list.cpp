#include "./bucket_list.hpp"

#include <iostream>
#include <limits>

using namespace std;

// Static members

int BucketList::invalid_gain_ = numeric_limits<int>::min();

// Public members

BucketList::BucketList(int num_all_cells, int num_all_nets, int num_all_pins)
    : offset_(num_all_pins),
      size_(num_all_pins * 2 + 1),
      max_gain_(invalid_gain_),
      cell_ids_(0),
      iterator_from_cell_id_(num_all_cells),
      cell_ids_from_offsetted_gain_(size_),
      gain_iterator_from_cell_id_(num_all_cells),
      cell_ids_from_net_id_(num_all_nets),
      net_id_iterators_from_cell_id_(num_all_cells),
      gain_from_cell_id_(num_all_cells, invalid_gain_),
      num_free_cells_from_offsetted_gain_(size_, 0) {}

void BucketList::Print(std::ostream& os, int num_spaces) const {
  os << string(num_spaces, ' ') << "gain_from_cell_id_: " << endl;
  for (int i = 0; i < gain_from_cell_id_.size(); ++i) {
    const int cell_id = i;
    cout << cell_id << " " << gain_from_cell_id_[i] << endl;
  }
}

bool BucketList::HasCell(int cell_id) const {
  return gain_from_cell_id_.at(cell_id) != invalid_gain_;
}

bool BucketList::AreAllCellsLocked() const {
  return max_gain_ == invalid_gain_;
}

int BucketList::NumCells() const { return cell_ids_.size(); }

int BucketList::NumNetCells(int net_id) const {
  return cell_ids_from_net_id_.at(net_id).size();
}

const list<int>& BucketList::CellIds() const { return cell_ids_; }

const list<int>& BucketList::NetCellIds(int net_id) const {
  return cell_ids_from_net_id_.at(net_id);
}

int BucketList::Gain(int cell_id) const {
  return gain_from_cell_id_.at(cell_id);
}

int BucketList::MaxGain() const { return max_gain_; }

int BucketList::MaxGainCellId() const {
  return CellIdsFromGain(max_gain_).front();
}

void BucketList::Reset() {
  max_gain_ = invalid_gain_;
  cell_ids_.clear();
  cell_ids_from_offsetted_gain_.assign(size_, list<int>());
  cell_ids_from_net_id_.assign(size_, list<int>());
  gain_from_cell_id_.assign(gain_from_cell_id_.size(), 0);
  num_free_cells_from_offsetted_gain_.assign(size_, 0);
}

void BucketList::InitializeCell(int cell_id, const std::vector<int>& net_ids) {
  cell_ids_.push_front(cell_id);
  iterator_from_cell_id_.at(cell_id) = cell_ids_.begin();
  for (int net_id : net_ids) {
    auto& net_cell_ids = cell_ids_from_net_id_.at(net_id);
    net_cell_ids.push_front(cell_id);
    net_id_iterators_from_cell_id_.at(cell_id).push_back(
        {net_id, net_cell_ids.begin()});
  }
  const int initial_gain = 0;
  InsertCell(cell_id, net_ids, initial_gain, false);
}

void BucketList::InsertCell(int cell_id, const std::vector<int>& net_ids) {
  InsertCell(cell_id, net_ids, 0, true);
}

void BucketList::DeleteCell(int cell_id) { DeleteCell(cell_id, true); }

void BucketList::UpdateCell(int cell_id, int new_gain) {
  DeleteCell(cell_id, false);
  InsertCell(cell_id, {}, new_gain, false);
}

// Private members

int BucketList::NumFreeCellsFromGain(int gain) const {
  return num_free_cells_from_offsetted_gain_.at(offset_ + gain);
}

const list<int>& BucketList::CellIdsFromGain(int gain) const {
  return cell_ids_from_offsetted_gain_.at(offset_ + gain);
}

list<int>& BucketList::CellIdsFromGain(int gain) {
  return cell_ids_from_offsetted_gain_.at(offset_ + gain);
}

void BucketList::IncrementNumFreeCellsFromGain(int gain) {
  ++num_free_cells_from_offsetted_gain_.at(offset_ + gain);
}

void BucketList::DecrementNumFreeCellsFromGain(int gain) {
  --num_free_cells_from_offsetted_gain_.at(offset_ + gain);
}

void BucketList::InsertCell(int cell_id, const vector<int>& net_ids, int gain,
                            bool is_locked) {
  if (is_locked) {
    cell_ids_.push_front(cell_id);
    iterator_from_cell_id_.at(cell_id) = cell_ids_.begin();
    for (int net_id : net_ids) {
      auto& net_cell_ids = cell_ids_from_net_id_.at(net_id);
      net_cell_ids.push_front(cell_id);
      net_id_iterators_from_cell_id_.at(cell_id).push_back(
          {net_id, net_cell_ids.begin()});
    }
  }
  auto& gain_cell_ids = CellIdsFromGain(gain);
  if (!is_locked) {
    gain_cell_ids.push_front(cell_id);
    gain_iterator_from_cell_id_.at(cell_id) = gain_cell_ids.begin();
    IncrementNumFreeCellsFromGain(gain);

    if (gain > max_gain_) {
      max_gain_ = gain;
    }
  } else {
    gain_cell_ids.push_back(cell_id);
    gain_iterator_from_cell_id_.at(cell_id) = prev(gain_cell_ids.end());
  }
  gain_from_cell_id_.at(cell_id) = gain;
}

void BucketList::DeleteCell(int cell_id, bool is_leaving) {
  if (is_leaving) {
    auto& it = iterator_from_cell_id_.at(cell_id);
    cell_ids_.erase(it);
    for (auto& net_id_it : net_id_iterators_from_cell_id_.at(cell_id)) {
      const int net_id = net_id_it.first;
      auto& net_it = net_id_it.second;
      cell_ids_from_net_id_.at(net_id).erase(net_it);
    }
  }
  auto& gain_it = gain_iterator_from_cell_id_.at(cell_id);
  const int gain = gain_from_cell_id_.at(cell_id);
  CellIdsFromGain(gain).erase(gain_it);
  DecrementNumFreeCellsFromGain(gain);
  if (gain == max_gain_ && NumFreeCellsFromGain(gain) == 0) {
    for (int i = gain - 1; i >= offset_ * (-1); --i) {
      if (NumFreeCellsFromGain(i) != 0) {
        max_gain_ = i;
        break;
      }
    }
    if (max_gain_ == gain) {
      max_gain_ = invalid_gain_;
    }
  }
  gain_from_cell_id_.at(cell_id) = invalid_gain_;
}
