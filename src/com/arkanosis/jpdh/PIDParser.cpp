#include <sstream>

#include "com/arkanosis/jpdh/PIDParser.hpp"

namespace jpdh {

  PIDParser::PIDParser()
    : _pid(0),
      _prefix(),
      _suffix(),
      _error(false),
      _errorMessage() {
  }

  bool PIDParser::parse(char const* fullPath) {
    _error = false;
    for (char const* first = fullPath;
         *first;
         ++first) {
      if (*first == '\\') {
        char const* last = ++first;
        if (*last++ == 'P' &&
            *last++ == 'I' &&
            *last++ == 'D' &&
            *last++ == '(') {
          _pid = 0;
          while (char digit = *last++) {
            if (digit == ')' &&
                *last == '\\') {
              if (!_pid) {
                _error = true;
                std::stringstream ss;
                ss << "Invalid PID in expression '" << std::string(first, last) << "'";
                _errorMessage.assign(ss.str());
                return false;
              }
              _prefix.assign(fullPath, std::distance(fullPath, first));
              _suffix.assign(last);
              return true;
            } else if (digit < '0' || digit > '9') {
              _error = true;
              std::stringstream ss;
              ss << "Invalid digit '" << digit << "' in PID in expression '" << std::string(first, last) << "'";
              _errorMessage.assign(ss.str());
              return false;
            }
            _pid *= 10;
            _pid += digit - '0';
          }
          _error = true;
          std::stringstream ss;
          ss << "Unexpected end of PID in expression '" << std::string(first, last - 1) << "'";
          _errorMessage.assign(ss.str());
          return false;
        }
      }
    }
    return false;
  }

  bool PIDParser::error() const {
    return _error;
  }

  std::string const& PIDParser::getErrorMessage() const {
    return _errorMessage;
  }

  int PIDParser::getPID() const {
    return _pid;
  }

  std::string const& PIDParser::getPrefix() const {
    return _prefix;
  }

  std::string const& PIDParser::getSuffix() const {
    return _suffix;
  }

} // namespace jpdh
