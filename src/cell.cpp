#include "./cell.hpp"

using namespace std;

Cell::Cell(const string& name)
    : name_(name), partition_name_("LEFT"), is_locked_(false), gain_(0) {}

const string& Cell::Name() const { return name_; }

const vector<int>& Cell::NetIds() const { return net_ids_; }

const string& Cell::PartitionName() const { return partition_name_; }

bool Cell::IsLocked() const { return is_locked_; }

int Cell::Gain() const { return gain_; }

void Cell::AddNetId(int net_id) { net_ids_.push_back(net_id); }

void Cell::Lock() { is_locked_ = true; }

void Cell::Unlock() { is_locked_ = false; }

void Cell::IncrementGain() { ++gain_; }

void Cell::DecrementGain() { --gain_; }
