#ifndef BUCKET_LIST_HPP
#define BUCKET_LIST_HPP

#include <iostream>
#include <list>
#include <vector>

class BucketList {
 public:
  BucketList(int num_all_cells, int num_all_nets, int num_all_pins);
  /* BucketList(const BucketList& other); */
  /* BucketList& operator=(const BucketList& other); */

  void Print(std::ostream& os = std::cout, int num_spaces = 0) const;

  bool HasCell(int cell_id) const;
  bool AreAllCellsLocked() const;
  int NumNetCells(int net_id) const;
  std::vector<int> CellIds() const;
  int Gain(int cell_id) const;
  int MaxGain() const;
  int MaxGainCellId() const;

  void InitializeCell(int cell_id, const std::vector<int>& net_ids);
  void InsertCell(int cell_id, const std::vector<int>& net_ids, int gain);
  void DeleteCell(int cell_id, const std::vector<int>& net_ids);
  void UpdateCell(int cell_id, int new_gain);

 private:
  static int invalid_gain_;
  static std::list<int> dummy_list_;

  const std::list<int>& CellIdsFromGain(int gain) const;
  int NumFreeCellsFromGain(int gain) const;
  std::list<int>& CellIdsFromGain(int gain);
  void IncrementNumFreeCellsFromGain(int gain);
  void DecrementNumFreeCellsFromGain(int gain);
  void InsertCell(int cell_id, const std::vector<int>& net_ids, int gain,
                  bool is_locked);
  void DeleteCell(int cell_id, const std::vector<int>& net_ids, bool is_locked);

  int offset_;
  int size_;
  int max_gain_;
  std::vector<std::list<int>> cell_ids_from_offsetted_gain_;
  std::vector<int> num_free_cells_from_offsetted_gain_;
  std::vector<int> gain_from_cell_id_;
  std::vector<std::list<int>::iterator> iterator_from_cell_id_;
  std::vector<int> num_net_cells_from_net_id_;
};

#endif
