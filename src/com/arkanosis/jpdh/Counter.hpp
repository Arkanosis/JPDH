#ifndef JPDH_COUNTER_HPP_
# define JPDH_COUNTER_HPP_

# include <string>

# include <jni.h>
# include <Pdh.h>

#include "com/arkanosis/jpdh/ProcessParser.hpp"

namespace jpdh {

  class Query;

  class Counter {
  public:
    Counter(char const* fullPath, ::PDH_HCOUNTER handle);
    virtual ~Counter();
    std::string const& getFullPath() const;
    virtual bool isValid() const;
    virtual bool getHandle(::JNIEnv* /* env */, ::PDH_HCOUNTER& handle);
    virtual bool removeHandle(::JNIEnv* env);
  protected:
    std::string const _fullPath;
    ::PDH_HCOUNTER _handle;
  };

  class ProcessCounter
    : public Counter {
  public:
    ProcessCounter(::JNIEnv* env, ::DWORD pid, std::string const& prefix, std::string const& suffix, char const* fullPath, Query const* query);
    virtual ~ProcessCounter();
    virtual bool isValid() const override;
    virtual bool getHandle(::JNIEnv* env, ::PDH_HCOUNTER& handle) override;
    virtual bool removeHandle(::JNIEnv* env) override;
  private:
    bool _setHandles(::JNIEnv* env);
  protected:
    ::DWORD const _pid;
    std::string _prefix;
    std::string _suffix;
    std::string _pidPath;
    Query const* _query;
    ::PDH_HCOUNTER _pidHandle;
    bool _valid;
    ProcessParser _processParser;
  };

} // namespace jpdh

#endif // !JPDH_COUNTER_HPP_
