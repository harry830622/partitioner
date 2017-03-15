#include "./net.hpp"

using namespace std;

Net::Net(const string& name) : name_(name) {}

const string& Net::Name() const { return name_; }

const vector<int>& Net::CellIds() const { return cell_ids_; }

void Net::ConnectCell(int cell_id) { cell_ids_.push_back(cell_id); }
