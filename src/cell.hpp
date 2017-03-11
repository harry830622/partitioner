#ifndef CELL_HPP
#define CELL_HPP

#include <string>
#include <vector>

class Cell {
 public:
  Cell(const std::string& name);

  const std::string& Name() const;
  const std::vector<int>& NetIds() const;
  const std::string& PartitionName() const;
  bool IsLocked() const;
  int Gain() const;

  void AddNetId(int net_id);
  void Lock();
  void Unlock();
  void IncrementGain();
  void DecrementGain();

 private:
  std::string name_;
  std::vector<int> net_ids_;

  std::string partition_name_;
  bool is_locked_;
  int gain_;
};

#endif
