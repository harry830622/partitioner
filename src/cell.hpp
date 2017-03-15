#ifndef CELL_HPP
#define CELL_HPP

#include <iostream>
#include <string>
#include <vector>

class Cell {
 public:
  Cell(const std::string& name);

  void Print(std::ostream& os = std::cout, int num_spaces = 0) const;

  const std::string& Name() const;
  const std::vector<int>& NetIds() const;
  bool IsLocked() const;
  int Gain() const;

  void ConnectNet(int net_id);
  void Lock();
  void Unlock();
  void IncrementGain();
  void DecrementGain();

 private:
  std::string name_;
  std::vector<int> net_ids_;

  bool is_locked_;
  int gain_;
};

#endif
