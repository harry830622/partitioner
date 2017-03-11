#include "./net.hpp"

using namespace std;

Net::Net(int id, const string& name) : id_(id), name_(name) {}

const string& Net::Name() const { return name_; }

void Net::AddCell(int cell_id) { cell_ids_.push_back(cell_id); }
