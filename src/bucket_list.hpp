#ifndef BUCKET_LIST_HPP
#define BUCKET_LIST_HPP

#include <list>
#include <vector>

class BucketList {
 public:
   BucketList(int num_cells, int num_pins);

   const std::list<int>& CellIds() const;

   void AddCell(int cell_id, int gain);
   void RemoveCell(int cell_id, int gain);

 private:
   int offset_;
   int max_gain_;
   std::vector<std::list<int>> cell_ids_from_gain_;
   std::vector<std::list<int>::iterator> list_iterator_from_cell_id_;
};

#endif
