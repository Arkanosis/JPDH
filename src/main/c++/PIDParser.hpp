#ifndef JPDH_PIDPARSER_HPP_
# define JPDH_PIDPARSER_HPP_

namespace jpdh {

  class PIDParser {
  public:
    PIDParser();
    bool parse(char const* fullPath);
    bool error() const;
    std::string const& getErrorMessage() const;
    int getPID() const;
    std::string const& getPrefix() const;
    std::string const& getSuffix() const;
  private:
    int _pid;
    std::string _prefix;
    std::string _suffix;
    bool _error;
    std::string _errorMessage;
  };

} // namespace jpdh

#endif // !JPDH_PIDPARSER_HPP_
