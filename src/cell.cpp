#include "./cell.hpp"

using namespace std;

Cell::Cell(const string& name) : name_(name), is_locked_(false) {}

void Cell::Print(ostream& os, int num_spaces) const {
  os << string(num_spaces, ' ') << "name_: " << name_ << endl;
  os << string(num_spaces, ' ') << "net_ids_: " << endl;
  for (int net_id : net_ids_) {
    os << string(num_spaces + 2, ' ') << net_id << endl;
  }
  os << string(num_spaces, ' ') << "is_locked_: " << is_locked_ << endl;
}

const string& Cell::Name() const { return name_; }

const vector<int>& Cell::NetIds() const { return net_ids_; }

bool Cell::IsLocked() const { return is_locked_; }

void Cell::ConnectNet(int net_id) { net_ids_.push_back(net_id); }

void Cell::Lock() { is_locked_ = true; }

void Cell::Unlock() { is_locked_ = false; }
