#ifndef BUCKET_LIST_HPP
#define BUCKET_LIST_HPP

#include <list>
#include <vector>

class BucketList {
 public:
  BucketList(int num_cells, int num_pins);

  int MaxGain() const;
  int MaxGainCellId() const;

  void FreeAllCells();
  void InsertCell(int cell_id, int gain, bool is_locked);
  void RemoveCell(int cell_id, int gain, bool is_locked);

 private:
  const std::list<int>& CellIdsFromGain(int gain) const;
  std::list<int>& CellIdsFromGain(int gain);

  int offset_;
  int size_;
  int max_gain_;
  std::vector<std::list<int>> cell_ids_from_offsetted_gain_;
  std::vector<int> num_free_cells_from_offsetted_gain_;
  std::vector<std::list<int>::iterator> list_iterator_from_cell_id_;
};

#endif
