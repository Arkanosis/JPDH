#include "com/arkanosis/jpdh/Counter.hpp"
#include "com/arkanosis/jpdh/Query.hpp"

namespace jpdh {

  Query::Query(char const* dataSource, ::PDH_HQUERY handle)
    : _dataSource(dataSource ? dataSource : ""),
      _handle(handle) {
  }

  Query::~Query() {
    for (auto counter : _counters) {
      delete counter.second;
    }
  }

  bool Query::hasDataSource() const {
    return _dataSource.length();
  }

  std::string const& Query::getDataSource() const {
    return _dataSource;
  }

  ::PDH_HQUERY Query::getHandle() const {
    return _handle;
  }

  bool Query::hasCounter(char const* counterFullPath) {
    return _counters.find(counterFullPath) != _counters.end();
  }

  void Query::add(Counter* counter) {
    _counters[counter->getFullPath()] = counter;
  }

  bool Query::remove(Counter* counter) {
    return _counters.erase(counter->getFullPath());
  }

} // namespace jpdh
