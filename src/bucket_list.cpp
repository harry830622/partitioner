#include "./bucket_list.hpp"

#include <iostream>
#include <limits>

using namespace std;

int BucketList::invalid_gain_ = numeric_limits<int>::min();
list<int> BucketList::dummy_list_;

BucketList::BucketList(int num_all_cells, int num_all_nets, int num_all_pins)
    : offset_(num_all_pins),
      size_(num_all_pins * 2 + 1),
      max_gain_(invalid_gain_),
      cell_ids_from_offsetted_gain_(size_),
      num_free_cells_from_offsetted_gain_(size_, 0),
      gain_from_cell_id_(num_all_cells, invalid_gain_),
      iterator_from_cell_id_(num_all_cells, dummy_list_.end()),
      num_net_cells_from_net_id_(num_all_nets) {}

/* BucketList::BucketList(const BucketList& other) */
/*     : offset_(other.offset_), */
/*       size_(other.size_), */
/*       max_gain_(invalid_gain_), */
/*       cell_ids_from_offsetted_gain_(other.cell_ids_from_offsetted_gain_), */
/*       num_free_cells_from_offsetted_gain_( */
/*           other.num_free_cells_from_offsetted_gain_), */
/*       gain_from_cell_id_(other.gain_from_cell_id_), */
/*       iterator_from_cell_id_(other.iterator_from_cell_id_.size(), */
/*                              dummy_list_.end()), */
/*       num_net_cells_from_net_id_(other.num_net_cells_from_net_id_) { */
/*   for (auto cell_ids : cell_ids_from_offsetted_gain_) { */
/*     for (auto it = cell_ids.begin(); it != cell_ids.end(); ++it) { */
/*       const int cell_id = *it; */
/*       iterator_from_cell_id_.at(cell_id) = it; */
/*     } */
/*   } */
/* } */

/* BucketList& BucketList::operator=(const BucketList& other) { */
/*   offset_ = other.offset_; */
/*   size_ = other.size_; */
/*   max_gain_ = other.max_gain_; */
/*   cell_ids_from_offsetted_gain_ = other.cell_ids_from_offsetted_gain_; */
/*   num_free_cells_from_offsetted_gain_ = */
/*       other.num_free_cells_from_offsetted_gain_; */
/*   gain_from_cell_id_ = other.gain_from_cell_id_; */
/*   iterator_from_cell_id_ = other.iterator_from_cell_id_; */
/*   num_net_cells_from_net_id_ = other.num_net_cells_from_net_id_; */

/*   return *this; */
/* } */

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

int BucketList::NumNetCells(int net_id) const {
  return num_net_cells_from_net_id_.at(net_id);
}

vector<int> BucketList::CellIds() const {
  vector<int> cell_ids;
  for (const auto& it : iterator_from_cell_id_) {
    if (it != dummy_list_.end()) {
      cell_ids.push_back(*it);
    }
  }

  return cell_ids;
}

int BucketList::Gain(int cell_id) const {
  return gain_from_cell_id_.at(cell_id);
}

int BucketList::MaxGain() const { return max_gain_; }

int BucketList::MaxGainCellId() const {
  return CellIdsFromGain(max_gain_).front();
}

void BucketList::InitializeCell(int cell_id, const std::vector<int>& net_ids) {
  const int initial_gain = 0;
  InsertCell(cell_id, net_ids, initial_gain, false);
}

void BucketList::InsertCell(int cell_id, const std::vector<int>& net_ids,
                            int gain) {
  InsertCell(cell_id, net_ids, gain, true);
}

void BucketList::DeleteCell(int cell_id, const std::vector<int>& net_ids) {
  DeleteCell(cell_id, net_ids, false);
}

void BucketList::UpdateCell(int cell_id, int new_gain) {
  DeleteCell(cell_id, {}, false);
  InsertCell(cell_id, {}, new_gain, false);
}

const list<int>& BucketList::CellIdsFromGain(int gain) const {
  return cell_ids_from_offsetted_gain_.at(offset_ + gain);
}

int BucketList::NumFreeCellsFromGain(int gain) const {
  return num_free_cells_from_offsetted_gain_.at(offset_ + gain);
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
  auto& cell_ids = CellIdsFromGain(gain);
  if (!is_locked) {
    cell_ids.push_front(cell_id);
    iterator_from_cell_id_.at(cell_id) = cell_ids.begin();
    IncrementNumFreeCellsFromGain(gain);

    if (gain > max_gain_) {
      max_gain_ = gain;
    }
  } else {
    cell_ids.push_back(cell_id);
    iterator_from_cell_id_.at(cell_id) = prev(cell_ids.end());
  }
  gain_from_cell_id_.at(cell_id) = gain;
  for (int net_id : net_ids) {
    ++num_net_cells_from_net_id_.at(net_id);
  }
}

void BucketList::DeleteCell(int cell_id, const vector<int>& net_ids,
                            bool is_locked) {
  auto& it = iterator_from_cell_id_.at(cell_id);
  const int gain = gain_from_cell_id_.at(cell_id);
  CellIdsFromGain(gain).erase(it);
  if (!is_locked) {
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
  }
  gain_from_cell_id_.at(cell_id) = invalid_gain_;
  iterator_from_cell_id_.at(cell_id) = dummy_list_.end();
  for (int net_id : net_ids) {
    --num_net_cells_from_net_id_.at(net_id);
  }
}
