#include "./partitioner.hpp"

#include <unordered_set>

using namespace std;

Partitioner::Partitioner(Database& database)
    : database_(database),
      partitions_(2, BucketList(database.NumCells(), database.NumNets(),
                                database.ComputeNumPins())) {
  const int num_all_cells = database_.NumCells();
  BucketList& left_partition = partitions_[0];
  BucketList& right_partition = partitions_[1];
  for (int i = 0; i < num_all_cells / 2; ++i) {
    const int cell_id = i;
    const Cell& cell = database_.CellFromId(cell_id);
    left_partition.InitializeCell(cell_id, cell.NetIds());
  }
  for (int i = num_all_cells / 2; i < num_all_cells; ++i) {
    const int cell_id = i;
    const Cell& cell = database_.CellFromId(cell_id);
    right_partition.InitializeCell(cell_id, cell.NetIds());
  }
  InitializeGains();
}

void Partitioner::PartitionCells() {
  const int num_all_cells = database_.NumCells();
  const int num_all_nets = database_.NumNets();
  const int num_all_pins = database_.ComputeNumPins();
  BucketList& left_partition = partitions_[0];
  BucketList& right_partition = partitions_[1];

  cout << "Start partitioning..." << endl;

  unordered_set<int> left_partition_cell_ids(left_partition.CellIds().begin(),
                                             left_partition.CellIds().end());
  unordered_set<int> right_partition_cell_ids(right_partition.CellIds().begin(),
                                             right_partition.CellIds().end());

  int nth_iteration = 0;
  int max_partial_sum = 0;
  do {
    ++nth_iteration;
    cout << nth_iteration << " th iteration" << endl;

    vector<int> moved_cell_ids(num_all_cells - 1, -1);
    int partial_sum = 0;

    max_partial_sum = -1 * num_all_pins - 1;
    int max_partial_sum_index = -1;

    for (int i = 0; i < num_all_cells - 1; ++i) {
      /* cout << "Left max gain: " << left_partition.MaxGain() << endl; */
      /* cout << "Left # of cells: " << left_partition.NumCells() << endl; */
      /* cout << "Right max gain: " << right_partition.MaxGain() << endl; */
      /* cout << "Right # of cells: " << right_partition.NumCells() << endl; */
      int cell_id = -1;
      int gain;
      if (ArePartitionsBalancedAfterMove(left_partition, right_partition) &&
          !left_partition.AreAllCellsLocked()) {
        cell_id = left_partition.MaxGainCellId();
        gain = left_partition.MaxGain();
      } else {
        cell_id = right_partition.MaxGainCellId();
        gain = right_partition.MaxGain();
      }

      /* const Cell& cell = database_.CellFromId(cell_id); */
      /* cout << "Choose cell " << cell.Name() << " with max gain " << gain */
      /*      << " is_locked: " << cell.IsLocked() << endl; */
      /* cout << "Partial sum: " << partial_sum << " Gain: " << gain << endl; */

      moved_cell_ids[i] = cell_id;
      partial_sum += gain;
      if (partial_sum > max_partial_sum) {
        max_partial_sum = partial_sum;
        max_partial_sum_index = i;
      }

      MoveCell(cell_id);
    }

    cout << "Max partial sum: " << max_partial_sum << endl;

    for (int i = 0; i <= max_partial_sum_index; ++i) {
      const int cell_id = moved_cell_ids.at(i);
      if (left_partition_cell_ids.count(cell_id) == 1) {
        left_partition_cell_ids.erase(cell_id);
        right_partition_cell_ids.insert(cell_id);
      } else {
        right_partition_cell_ids.erase(cell_id);
        left_partition_cell_ids.insert(cell_id);
      }
    }
    left_partition = BucketList(num_all_cells, num_all_nets, num_all_pins);
    right_partition = BucketList(num_all_cells, num_all_nets, num_all_pins);
    for (int cell_id : left_partition_cell_ids) {
      const Cell& cell = database_.CellFromId(cell_id);
      left_partition.InitializeCell(cell_id, cell.NetIds());
    }
    for (int cell_id : right_partition_cell_ids) {
      const Cell& cell = database_.CellFromId(cell_id);
      right_partition.InitializeCell(cell_id, cell.NetIds());
    }
    InitializeGains();

    /* cout << partitions_[0].CellIds().size() << " " */
    /*      << left_partition.CellIds().size() << endl; */
    /* cout << partitions_[1].CellIds().size() << " " */
    /*      << right_partition.CellIds().size() << endl; */

    UnlockAllCells();
  } while (max_partial_sum > 0);
}

bool Partitioner::ArePartitionsBalancedAfterMove(
    const BucketList& from_partition, const BucketList& to_partition) const {
  const int num_from_partition_cells = from_partition.NumCells();
  const int num_to_partition_cells = to_partition.NumCells();
  const int num_all_cells = database_.NumCells();
  const double balance_factor = database_.BalanceFactor();
  if (num_from_partition_cells - 1 < (1 - balance_factor) / 2 * num_all_cells ||
      num_to_partition_cells + 1 > (1 + balance_factor) / 2 * num_all_cells) {
    return false;
  }

  return true;
}

void Partitioner::InitializeGains() {
  const int num_all_cells = database_.NumCells();
  const int initial_gain = 0;
  BucketList& left_partition = partitions_[0];
  BucketList& right_partition = partitions_[1];
  vector<int> gain_from_cell_id(num_all_cells, initial_gain);
  for (int i = 0; i < num_all_cells; ++i) {
    const int cell_id = i;
    const Cell& cell = database_.CellFromId(cell_id);
    BucketList& from_partition =
        left_partition.HasCell(cell_id) ? left_partition : right_partition;
    BucketList& to_partition =
        left_partition.HasCell(cell_id) ? right_partition : left_partition;
    for (int net_id : cell.NetIds()) {
      const int num_from_partition_cells = from_partition.NumNetCells(net_id);
      const int num_to_partition_cells = to_partition.NumNetCells(net_id);
      if (num_from_partition_cells == 1) {
        ++gain_from_cell_id.at(cell_id);
      } else if (num_to_partition_cells == 0) {
        --gain_from_cell_id.at(cell_id);
      }
    }
  }
  for (int i = 0; i < gain_from_cell_id.size(); ++i) {
    const int cell_id = i;
    const int gain = gain_from_cell_id.at(cell_id);
    /* cout << database_.CellFromId(cell_id).Name() << " " << gain << endl; */
    if (left_partition.HasCell(cell_id)) {
      left_partition.UpdateCell(cell_id, gain);
    } else {
      right_partition.UpdateCell(cell_id, gain);
    }
  }
}

void Partitioner::UpdateGains(const vector<int>& gain_from_cell_id,
                              const vector<int>& old_gain_from_cell_id) {
  BucketList& left_partition = partitions_[0];
  BucketList& right_partition = partitions_[1];
  for (int i = 0; i < gain_from_cell_id.size(); ++i) {
    const int cell_id = i;
    const int gain = gain_from_cell_id.at(cell_id);
    const int old_gain = old_gain_from_cell_id.at(cell_id);
    const Cell& cell = database_.CellFromId(cell_id);
    if (gain != old_gain && !cell.IsLocked()) {
      BucketList& partition =
          left_partition.HasCell(cell_id) ? left_partition : right_partition;
      partition.UpdateCell(cell_id, gain);
    }
  }
}

void Partitioner::MoveCell(int cell_id) {
  const int num_all_cells = database_.NumCells();
  BucketList& left_partition = partitions_[0];
  BucketList& right_partition = partitions_[1];

  vector<int> gain_from_cell_id(num_all_cells);
  for (int left_partition_cell_id : left_partition.CellIds()) {
    const int gain = left_partition.Gain(left_partition_cell_id);
    gain_from_cell_id.at(left_partition_cell_id) = gain;
  }
  for (int right_partition_cell_id : right_partition.CellIds()) {
    const int gain = right_partition.Gain(right_partition_cell_id);
    gain_from_cell_id.at(right_partition_cell_id) = gain;
  }
  vector<int> old_gain_from_cell_id(gain_from_cell_id);

  BucketList& from_partition =
      left_partition.HasCell(cell_id) ? left_partition : right_partition;
  BucketList& to_partition =
      left_partition.HasCell(cell_id) ? right_partition : left_partition;
  Cell& cell = database_.CellFromId(cell_id);

  for (int net_id : cell.NetIds()) {
    const int num_to_partition_cells = to_partition.NumNetCells(net_id);
    const Net& net = database_.NetFromId(net_id);
    if (num_to_partition_cells == 0) {
      for (int net_cell_id : net.CellIds()) {
        const Cell& net_cell = database_.CellFromId(net_cell_id);
        if (!net_cell.IsLocked() && net_cell_id != cell_id) {
          ++gain_from_cell_id.at(net_cell_id);
        }
      }
    } else if (num_to_partition_cells == 1) {
      const int net_cell_id = to_partition.NetCellIds(net_id).front();
      const Cell& net_cell = database_.CellFromId(net_cell_id);
      if (!net_cell.IsLocked()) {
        --gain_from_cell_id.at(net_cell_id);
      }
    }
  }

  from_partition.DeleteCell(cell_id);
  cell.Lock();
  to_partition.InsertCell(cell_id, cell.NetIds());

  for (int net_id : cell.NetIds()) {
    const int num_from_partition_cells = from_partition.NumNetCells(net_id);
    const Net& net = database_.NetFromId(net_id);
    if (num_from_partition_cells == 0) {
      for (int net_cell_id : net.CellIds()) {
        const Cell& net_cell = database_.CellFromId(net_cell_id);
        if (!net_cell.IsLocked() && net_cell_id != cell_id) {
          --gain_from_cell_id.at(net_cell_id);
        }
      }
    } else if (num_from_partition_cells == 1) {
      const int net_cell_id = from_partition.NetCellIds(net_id).front();
      const Cell& net_cell = database_.CellFromId(net_cell_id);
      if (!net_cell.IsLocked()) {
        ++gain_from_cell_id.at(net_cell_id);
      }
    }
  }

  UpdateGains(gain_from_cell_id, old_gain_from_cell_id);
}

void Partitioner::UnlockAllCells() {
  const int num_all_cells = database_.NumCells();
  for (int i = 0; i < num_all_cells; ++i) {
    const int cell_id = i;
    database_.CellFromId(cell_id).Unlock();
  }
}
