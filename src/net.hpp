#ifndef NET_HPP
#define NET_HPP

#include <string>
#include <vector>

class Net {
 public:
  Net(int id, const std::string& name);

  const std::string& Name() const;

  void AddCell(int cell_id);

 private:
  int id_;
  std::string name_;
  std::vector<int> cell_ids_;
};

#endif
