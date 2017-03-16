#include "./bucket_list.hpp"

using namespace std;

BucketList::BucketList(int num_cells, int num_pins)
    : offset_(num_pins),
      size_(num_pins * 2 + 1),
      cell_ids_from_offsetted_gain_(size_),
      num_free_cells_from_offsetted_gain_(size_, 0),
      list_iterator_from_cell_id_(num_cells) {}

int BucketList::MaxGain() const { return max_gain_; }

int BucketList::MaxGainCellId() const {
  return CellIdsFromGain(max_gain_).front();
}

void BucketList::FreeAllCells() {
  for (int i = 0; i < cell_ids_from_offsetted_gain_.size(); ++i) {
    num_free_cells_from_offsetted_gain_[i] =
        cell_ids_from_offsetted_gain_[i].size();
  }
}

void BucketList::InsertCell(int cell_id, int gain, bool is_locked) {
  auto& cell_ids = CellIdsFromGain(gain);

  if (!is_locked) {
    cell_ids.push_front(cell_id);
    list_iterator_from_cell_id_.at(cell_id) = cell_ids.begin();

    ++num_free_cells_from_offsetted_gain_.at(gain + offset_);

    if (gain > max_gain_) {
      max_gain_ = gain;
    }
  } else {
    cell_ids.push_back(cell_id);
    list_iterator_from_cell_id_.at(cell_id) = prev(cell_ids.end());
  }
}

void BucketList::RemoveCell(int cell_id, int gain, bool is_locked) {
  auto& cell_ids = CellIdsFromGain(gain);

  auto& it = list_iterator_from_cell_id_.at(cell_id);
  cell_ids.erase(it);

  if (!is_locked) {
    --num_free_cells_from_offsetted_gain_.at(gain + offset_);

    if (gain == max_gain_ &&
        num_free_cells_from_offsetted_gain_.at(gain + offset_) == 0) {
      for (int i = max_gain_ - 1; i >= -1 * offset_; --i) {
        if (num_free_cells_from_offsetted_gain_.at(i + offset_) != 0) {
          max_gain_ = i;
          break;
        }
      }
    }
  }
}

const list<int>& BucketList::CellIdsFromGain(int gain) const {
  return cell_ids_from_offsetted_gain_.at(offset_ + gain);
}

list<int>& BucketList::CellIdsFromGain(int gain) {
  return cell_ids_from_offsetted_gain_.at(offset_ + gain);
}
