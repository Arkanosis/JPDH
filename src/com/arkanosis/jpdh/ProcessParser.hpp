#ifndef JPDH_PROCESSPARSER_HPP_
# define JPDH_PROCESSPARSER_HPP_

namespace jpdh {

  class ProcessParser {
  public:
    explicit ProcessParser(std::string const& processCounter);
    bool parse(char const* fullPath);
    bool error() const;
    std::string const& getErrorMessage() const;
    std::string const& getProcess() const;
  private:
    std::string const& _processCounter;
    std::string _process;
    bool _error;
    std::string _errorMessage;
  };

} // namespace jpdh

#endif // !JPDH_PROCESSPARSER_HPP_
