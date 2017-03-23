#ifndef BUCKET_LIST_HPP
#define BUCKET_LIST_HPP

#include <list>
#include <vector>

class BucketList {
 public:
  static int invalid_gain_;

  BucketList(int num_all_cells, int num_all_nets, int num_all_pins);

  bool HasCell(int cell_id) const;
  bool AreAllCellsLocked() const;
  int NumCells() const;
  int NumNetCells(int net_id) const;
  int Gain(int cell_id) const;
  int MaxGain() const;
  int MaxGainCellId() const;

  void InsertCell(int cell_id, const std::vector<int>& net_ids, int gain,
                  bool is_locked = true);
  void DeleteCell(int cell_id, const std::vector<int>& net_ids,
                  bool is_locked = false);
  void UpdateCell(int cell_id, int new_gain, bool is_locked);

 private:
  static std::list<int> dummy_list_;

  const std::list<int>& CellIdsFromGain(int gain) const;
  int NumFreeCellsFromGain(int gain) const;
  std::list<int>& CellIdsFromGain(int gain);
  void IncrementNumFreeCellsFromGain(int gain);
  void DecrementNumFreeCellsFromGain(int gain);

  const int offset_;
  const int size_;
  int max_gain_;
  std::vector<std::list<int>> cell_ids_from_offsetted_gain_;
  std::vector<int> num_free_cells_from_offsetted_gain_;
  std::vector<int> gain_from_cell_id_;
  std::vector<std::list<int>::iterator> iterator_from_cell_id_;
  std::vector<int> num_net_cells_from_net_id_;
};

#endif
