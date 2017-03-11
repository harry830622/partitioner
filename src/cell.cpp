#include "./cell.hpp"

using namespace std;

Cell::Cell(int id, const string& name, int size)
    : id_(id), name_(name), size_(size) {}

const string& Cell::Name() const { return name_; }

void Cell::AddNet(int net_id) { net_ids_.push_back(net_id); }
