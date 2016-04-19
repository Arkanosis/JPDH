#ifndef JPDH_QUERY_HPP_
# define JPDH_QUERY_HPP_

# include <string>
# include <unordered_map>

# include <Pdh.h>

namespace jpdh {

  class Counter;

  class Query {
  public:
    Query(char const* dataSource, ::PDH_HQUERY handle);
    ~Query();
    bool hasDataSource() const;
    std::string const& getDataSource() const;
    ::PDH_HQUERY getHandle() const;
    bool hasCounter(char const* counterFullPath);
    void add(Counter* counter);
    bool remove(Counter* counter);
  private:
    std::string const _dataSource;
    ::PDH_HQUERY _handle;
    std::unordered_map<std::string, Counter*> _counters;
  };

} // namespace jpdh

#endif // !JPDH_QUERY_HPP_
