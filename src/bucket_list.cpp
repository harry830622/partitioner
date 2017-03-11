#include "./bucket_list.hpp"

using namespace std;

BucketList::BucketList(int num_pins)
    : offset_(num_pins), cell_ids_from_gain_(num_pins * 2 + 1) {}

const list<int>& BucketList::CellIds() const {
  return cell_ids_from_gain_.at(max_gain_ + offset_);
}

void BucketList::AddCellId(int cell_id, int gain) {
  cell_ids_from_gain_.at(gain + offset_).push_back(cell_id);
}

void BucketList::RemoveCellId(const list<int>::iterator& it, int gain) {
  cell_ids_from_gain_.at(gain + offset_).erase(it);
}
