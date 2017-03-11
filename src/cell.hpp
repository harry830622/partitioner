#ifndef CELL_HPP
#define CELL_HPP

#include <string>
#include <vector>
#include <functional>

class Cell {
 public:
  Cell(int id, const std::string& name, int size = 1);

  const std::string& Name() const;
  void ForEachNet(std::function<void(int net_id)> handler) const;
  bool IsLocked() const;

  void AddNet(int net_id);
  void Move();
  void Unlock();
  void IncrementGain();
  void DecrementGain();

 private:
  int id_;
  std::string name_;
  int size_;
  std::vector<int> net_ids_;

  bool is_locked_;
  int gain_;
};

#endif
