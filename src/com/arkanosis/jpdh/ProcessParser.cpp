#include <sstream>

#include "com/arkanosis/jpdh/ProcessParser.hpp"

namespace jpdh {

  ProcessParser::ProcessParser(std::string const& processCounter)
    : _processCounter(processCounter),
      _process(),
      _error(false),
      _errorMessage() {
  }

  bool ProcessParser::parse(char const* fullPath) {
    _error = false;
    for (char const* first = fullPath;
         *first;
         ++first) {
      if (!_processCounter.compare(0, std::string::npos, first, _processCounter.length())) {
        char const* last = first + _processCounter.length();
        char const* name = last;
        while (char character = *last++) {
          if (character == ')' &&
              *last == '\\') {
            if (last == name + 1) {
              _error = true;
              std::stringstream ss;
              ss << "Invalid process name in expression '" << std::string(first, last) << "'";
              _errorMessage.assign(ss.str());
              return false;
            }
            _process.assign(name, std::distance(name, last) - 1);
            return true;
          }
        }
        _error = true;
        std::stringstream ss;
        ss << "Unexpected end of process in expression '" << std::string(first, last - 1) << "'";
        _errorMessage.assign(ss.str());
        return false;
      }
    }
    return false;
  }

  bool ProcessParser::error() const {
    return _error;
  }

  std::string const& ProcessParser::getErrorMessage() const {
    return _errorMessage;
  }

  std::string const& ProcessParser::getProcess() const {
    return _process;
  }

} // namespace jpdh
