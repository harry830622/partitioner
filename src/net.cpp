#include "./net.hpp"

using namespace std;

Net::Net(const string& name) : name_(name) {}

void Net::Print(ostream& os, int num_spaces) const {
  os << string(num_spaces, ' ') << "name_: " << name_ << endl;
  os << string(num_spaces, ' ') << "cell_ids_: " << endl;
  for (int cell_id : cell_ids_) {
    os << string(num_spaces + 2, ' ') << cell_id << endl;
  }
}

const string& Net::Name() const { return name_; }

const vector<int>& Net::CellIds() const { return cell_ids_; }

void Net::ConnectCell(int cell_id) { cell_ids_.push_back(cell_id); }
