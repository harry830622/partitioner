#include "./bucket_list.hpp"

#include <limits>

using namespace std;

int BucketList::invalid_gain_ = numeric_limits<int>::min();
list<int> BucketList::dummy_list_;

BucketList::BucketList(int num_all_cells, int num_all_pins,
                       const vector<int>& cell_ids)
    : offset_(num_all_pins),
      size_(num_all_pins * 2 + 1),
      max_gain_(invalid_gain_),
      cell_ids_from_offsetted_gain_(size_),
      num_free_cells_from_offsetted_gain_(size_, 0),
      gain_from_cell_id_(num_all_cells, invalid_gain_),
      iterator_from_cell_id_(num_all_cells, dummy_list_.end()) {
  const int initial_gain = 0;
  for (int cell_id : cell_ids) {
    InsertCell(cell_id, initial_gain, false);
  }
  max_gain_ = initial_gain;
}

bool BucketList::HasCell(int cell_id) const {
  return iterator_from_cell_id_.at(cell_id) != dummy_list_.end();
}

bool BucketList::AreAllCellsLocked() const {
  return max_gain_ == invalid_gain_;
}

int BucketList::NumCells() const {
  int num_cells = 0;
  for (const auto& it : iterator_from_cell_id_) {
    if (it != dummy_list_.end()) {
      ++num_cells;
    }
  }

  return num_cells;
}

int BucketList::Gain(int cell_id) const {
  return gain_from_cell_id_.at(cell_id);
}

int BucketList::MaxGain() const { return max_gain_; }

int BucketList::MaxGainCellId() const {
  return CellIdsFromGain(max_gain_).front();
}

void BucketList::InsertCell(int cell_id, int gain, bool is_locked) {
  auto& cell_ids = CellIdsFromGain(gain);
  if (!is_locked) {
    cell_ids.push_front(cell_id);
    iterator_from_cell_id_.at(cell_id) = cell_ids.begin();
    IncrementNumFreeCellsFromGain(gain);
  } else {
    cell_ids.push_back(cell_id);
    iterator_from_cell_id_.at(cell_id) = prev(cell_ids.end());
  }
  gain_from_cell_id_.at(cell_id) = gain;
}

void BucketList::DeleteCell(int cell_id, bool is_locked) {
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
}

void BucketList::UpdateCell(int cell_id, int new_gain, bool is_locked) {
  DeleteCell(cell_id, is_locked);
  InsertCell(cell_id, new_gain, is_locked);
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
