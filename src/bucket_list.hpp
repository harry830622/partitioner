#ifndef BUCKET_LIST_HPP
#define BUCKET_LIST_HPP

#include <list>
#include <vector>

class BucketList {
 public:
   BucketList(int num_pins);

   const std::list<int>& CellIds() const;

   void AddCellId(int cell_id, int gain);
   void RemoveCellId(const std::list<int>::iterator& it, int gain);

 private:
   int offset_;
   int max_gain_;
   std::vector<std::list<int>> cell_ids_from_gain_;
};

#endif
