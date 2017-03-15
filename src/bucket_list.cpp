#include "./bucket_list.hpp"

using namespace std;

BucketList::BucketList(int num_cells, int num_pins)
    : offset_(num_pins),
      cell_ids_from_gain_(num_pins * 2 + 1),
      list_iterator_from_cell_id_(num_cells) {}

const list<int>& BucketList::CellIds() const {
  return cell_ids_from_gain_.at(max_gain_ + offset_);
}

void BucketList::AddCell(int cell_id, int gain) {
  auto& cell_ids = cell_ids_from_gain_.at(gain + offset_);
  cell_ids.push_back(cell_id);
  list_iterator_from_cell_id_.at(cell_id) = prev(cell_ids.end());

  if (gain > max_gain_) {
    max_gain_ = gain;
  }
}

void BucketList::RemoveCell(int cell_id, int gain) {
  auto& cell_ids = cell_ids_from_gain_.at(gain + offset_);
  auto& it = list_iterator_from_cell_id_.at(cell_id);
  cell_ids.erase(it);

  if (gain == max_gain_ && cell_ids.empty()) {
    for (int i = gain; i >= 0; --i) {
      if (!cell_ids_from_gain_[i].empty()) {
        max_gain_ = i;
        break;
      }
    }
  }
}
