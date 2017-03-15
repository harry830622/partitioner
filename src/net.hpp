#ifndef NET_HPP
#define NET_HPP

#include <string>
#include <vector>

class Net {
 public:
  Net(const std::string& name);

  const std::string& Name() const;
  const std::vector<int>& CellIds() const;

  void ConnectCell(int cell_id);

 private:
  std::string name_;
  std::vector<int> cell_ids_;
};

#endif
