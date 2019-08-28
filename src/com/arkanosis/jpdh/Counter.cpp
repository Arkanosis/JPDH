#include <memory>
#include <sstream>

#include <Pdh.h>
#include <PdhMsg.h>

#include "com/arkanosis/jpdh/Counter.hpp"
#include "com/arkanosis/jpdh/Query.hpp"
#include "com/arkanosis/jpdh/utils.hpp"

namespace jpdh {

  namespace {

    struct CounterInfo {
      int offset;
      ::PDH_HCOUNTER handle;
    };

    class AutoCloseableQuery {
    public:
      explicit AutoCloseableQuery(::JNIEnv* env, ::PDH_HQUERY query)
        : _env(env),
          _query(query) {
      }
      ~AutoCloseableQuery() {
        ::PDH_STATUS status = ::PdhCloseQuery(_query);
        if (status != ERROR_SUCCESS) {
          THROW_EXCEPTION(_env, status);
        }
      }
    private:
      ::JNIEnv* _env;
      ::PDH_HQUERY const _query;
    };

  } // namespace

  Counter::Counter(char const* fullPath, ::PDH_HCOUNTER handle)
    : _fullPath(fullPath),
      _handle(handle) {
  }

  Counter::~Counter() {
  }

  std::string const& Counter::getFullPath() const {
    return _fullPath;
  }

  bool Counter::isValid() const {
    return true;
  }

  bool Counter::getHandle(::JNIEnv* /* env */, ::PDH_HCOUNTER& handle) {
    handle = _handle;
    return true;
  }

  bool Counter::removeHandle(::JNIEnv* env) {
    ::PDH_STATUS status = ::PdhRemoveCounter(_handle);
    if (status != ERROR_SUCCESS) {
      THROW_EXCEPTION(env, status);
      return false;
    }
    return true;
  }

  std::string ProcessCounter::_pidPattern;
  std::string ProcessCounter::_processCounter;

  void ProcessCounter::_fillLocalizedStrings(::JNIEnv* env) {
    ::PDH_HQUERY query;
    ::PDH_STATUS status = ::PdhOpenQuery(nullptr, 0, &query);
    if (status != ERROR_SUCCESS) {
      THROW_EXCEPTION(env, status);
      return;
    }
    AutoCloseableQuery autoCloseableQuery(env, query);
    ::PDH_HCOUNTER counterHandle;
    status = ::PdhAddEnglishCounter(query, "\\Process(*)\\ID Process", 0, &counterHandle);
    if (status != ERROR_SUCCESS) {
      THROW_EXCEPTION(env, status);
      return;
    }
    DWORD bufferSize = 0;
    status = ::PdhGetCounterInfo(counterHandle, false, &bufferSize, nullptr);
    if (status != PDH_MORE_DATA) {
      THROW_EXCEPTION(env, status);
      return;
    }
    std::unique_ptr<char[]> counterInfoBytes(new char[bufferSize]);
    auto counterInfo = reinterpret_cast<PDH_COUNTER_INFO*>(counterInfoBytes.get());
    status = ::PdhGetCounterInfo(counterHandle, false, &bufferSize, counterInfo);
    if (status != ERROR_SUCCESS) {
      THROW_EXCEPTION(env, status);
      return;
    }
    _pidPattern = counterInfo->szFullPath + 1;
    ::PdhRemoveCounter(counterHandle);
    if (status != ERROR_SUCCESS) {
      THROW_EXCEPTION(env, status);
      return;
    }
    std::size_t processCounterLength = 0;
    while (counterInfo->szFullPath[processCounterLength] != '(') {
      ++processCounterLength;
    }
    _processCounter.assign(counterInfo->szFullPath, processCounterLength + 1);
  }

  std::string const& ProcessCounter::_getPidPattern(::JNIEnv* env) {
    if (_pidPattern.empty()) {
      _fillLocalizedStrings(env);
    }
    return _pidPattern;
  }

  std::string const& ProcessCounter::_getProcessCounter(::JNIEnv* env) {
    if (_pidPattern.empty()) {
      _fillLocalizedStrings(env);
    }
    return _processCounter;
  }

  ProcessCounter::ProcessCounter(::JNIEnv* env, ::DWORD pid, std::string const& prefix, std::string const& suffix, char const* fullPath, Query const* query)
    : Counter(fullPath, 0),
      _pid(pid),
      _prefix(prefix),
      _suffix(suffix),
      _pidPath(prefix + _getPidPattern(env)),
      _query(query),
      _pidHandle(0),
      _valid(false),
      _processParser(_getProcessCounter(env)) {
    if (!_setHandles(env)) {
      if (_valid) {
        std::stringstream ss;
        ss << "No process found for PID " << _pid;
        THROW("com/arkanosis/jpdh/NoSuchPIDException", env, ss.str().c_str());
        _valid = false;
      }
    }
  }

  ProcessCounter::~ProcessCounter() {
  }

  bool ProcessCounter::isValid() const {
    return _valid;
  }

  bool ProcessCounter::getHandle(::JNIEnv* env, ::PDH_HCOUNTER& handle) {
    for (; ; ) {
      DWORD type;
      ::PDH_FMT_COUNTERVALUE currentPid;
      ::PDH_STATUS status = ::PdhGetFormattedCounterValue(_pidHandle, PDH_FMT_LONG, &type, &currentPid);
      if (status == ERROR_SUCCESS) {
        if (currentPid.CStatus != PDH_CSTATUS_VALID_DATA && currentPid.CStatus != PDH_CSTATUS_NEW_DATA) {
          THROW_EXCEPTION(env, currentPid.CStatus);
          return false;
        }
        if (currentPid.longValue == _pid) {
          break;
        }
      }
      status = ::PdhRemoveCounter(_pidHandle);
      if (status != ERROR_SUCCESS) {
        THROW_EXCEPTION(env, status);
        return false;
      }
      status = ::PdhRemoveCounter(_handle);
      if (status != ERROR_SUCCESS) {
        THROW_EXCEPTION(env, status);
        return false;
      }
      if (!_setHandles(env)) {
        if (_valid) {
          std::stringstream ss;
          ss << "Process with PID " << _pid << " is not available anymore";
          THROW("com/arkanosis/jpdh/NoSuchPIDException", env, ss.str().c_str());
          _valid = false;
        }
        return false;
      }
      status = ::PdhCollectQueryData(_query->getHandle());
      if (status != ERROR_SUCCESS) {
        THROW_EXCEPTION(env, status);
        return false;
      }
    }
    handle = _handle;
    return true;
  }

  bool ProcessCounter::removeHandle(::JNIEnv* env) {
    ::PDH_STATUS status = ::PdhRemoveCounter(_pidHandle);
    if (status != ERROR_SUCCESS) {
      THROW_EXCEPTION(env, status);
      return false;
    }
    status = ::PdhRemoveCounter(_handle);
    if (status != ERROR_SUCCESS) {
      THROW_EXCEPTION(env, status);
      return false;
    }
    return true;
  }

  bool ProcessCounter::_setHandles(::JNIEnv* env) {
    DWORD bufferSize = 0;
    ::PDH_STATUS status = ::PdhExpandWildCardPath(_query->hasDataSource() ? _query->getDataSource().c_str() : nullptr, _pidPath.c_str(), nullptr, &bufferSize, 0);
    if (status != PDH_MORE_DATA) {
      THROW_EXCEPTION(env, status);
      return false;
    }
    std::unique_ptr<char[]> paths;
    do {
      paths = std::unique_ptr<char[]>(new char[bufferSize + 2]);
      status = ::PdhExpandWildCardPath(_query->hasDataSource() ? _query->getDataSource().c_str() : nullptr, _pidPath.c_str(), paths.get(), &bufferSize, 0);
    } while (status == PDH_MORE_DATA);
    if (status != ERROR_SUCCESS) {
      THROW_EXCEPTION(env, status);
      return false;
    }
    ::PDH_HQUERY query;
    status = ::PdhOpenQuery(_query->hasDataSource() ? _query->getDataSource().c_str() : nullptr, 0, &query);
    if (status != ERROR_SUCCESS) {
      THROW_EXCEPTION(env, status);
      return false;
    }
    AutoCloseableQuery autoCloseableQuery(env, query);
    std::vector<CounterInfo> counters;
    for (int offset = 0;
         paths[offset];
         offset += std::strlen(paths.get() + offset) + 1) {
      ::PDH_HCOUNTER counterHandle;
      status = ::PdhAddCounter(query, paths.get() + offset, 0, &counterHandle);
      if (status == ERROR_SUCCESS) {
        counters.emplace_back(CounterInfo{offset, counterHandle});
      }
    }
    status = ::PdhCollectQueryData(query);
    if (status != ERROR_SUCCESS) {
      THROW_EXCEPTION(env, status);
      return false;
    }
    for (const auto& counter: counters) {
      DWORD type;
      ::PDH_FMT_COUNTERVALUE value;
      status = ::PdhGetFormattedCounterValue(counter.handle, PDH_FMT_LONG, &type, &value);
      if (status == ERROR_SUCCESS) {
        if (value.CStatus != PDH_CSTATUS_VALID_DATA && value.CStatus != PDH_CSTATUS_NEW_DATA) {
          THROW_EXCEPTION(env, value.CStatus);
          return false;
        }
        if (value.longValue == _pid) {
          status = ::PdhAddCounter(_query->getHandle(), paths.get() + counter.offset, 0, &_pidHandle);
          if (status != ERROR_SUCCESS) {
            std::stringstream ss;
            ss << "Process with PID " << _pid << " is not available anymore";
            THROW("com/arkanosis/jpdh/NoSuchPIDException", env, ss.str().c_str());
            return false;
          }
          if (!_processParser.parse(paths.get() + counter.offset)) {
            std::stringstream ss;
            ss << "Unable to get process name for PID " << _pid;
            if (_processParser.error()) {
              ss << ": " << _processParser.getErrorMessage();
            }
            THROW("com/arkanosis/jpdh/JPDHException", env, ss.str().c_str());
            return false;
          }
          std::stringstream fullPath;
          fullPath << _prefix << "Process(" << _processParser.getProcess() << ")" << _suffix;
          status = ::PdhAddEnglishCounter(_query->getHandle(), fullPath.str().c_str(), 0, &_handle);
          if (status != ERROR_SUCCESS) {
            std::stringstream ss;
            ss << "Process with PID " << _pid << " is not available anymore";
            THROW("com/arkanosis/jpdh/NoSuchPIDException", env, ss.str().c_str());
            return false;
          }
          _valid = true;
          return true;
        }
      } // TODO  else { ::PdhRemoveCounter(counter.handle) and remove counter from counters }
    }
    _valid = true;
    return false;
  }

} // namespace jpdh
