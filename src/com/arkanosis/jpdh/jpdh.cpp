#include <jni.h>
#include <Pdh.h>
#include <PdhMsg.h>

#include "com/arkanosis/jpdh/jpdh.h"
#include "com/arkanosis/jpdh/utils.hpp"
#include "com/arkanosis/jpdh/Counter.hpp"
#include "com/arkanosis/jpdh/PIDParser.hpp"
#include "com/arkanosis/jpdh/Query.hpp"

namespace {

  class JNIString {
  public:
    JNIString(::JNIEnv* env, ::jstring str)
      : _env(env),
        _str(str),
        _chars(nullptr) {
      if (str) {
        _chars = env->GetStringUTFChars(str, nullptr);
      }
    }
    ~JNIString() {
      if (_chars) {
        _env->ReleaseStringUTFChars(_str, _chars);
      }
    }
    operator char const*() const {
      return _chars;
    }
    char const* get() const {
      return _chars;
    }
  private:
    ::JNIEnv* _env;
    ::jstring _str;
    char const* _chars;
  };

  template <typename T>
  ::jobject newObject(::JNIEnv* env, char const* className, T value) {
    ::jclass class_ = env->FindClass(className);
    ::jmethodID constructor = env->GetMethodID(class_, "<init>", "()V");
    ::jfieldID nativeRefField = env->GetFieldID(class_, "__native_ref", "J");
    ::jobject object_ = env->NewObject(class_, constructor);
    env->SetLongField(object_, nativeRefField, reinterpret_cast<::jlong>(value));
    return object_;
  }

  template <typename T>
  T getObject(::JNIEnv* env, ::jobject object_) {
    ::jclass class_ = env->GetObjectClass(object_);
    ::jfieldID nativeRefField = env->GetFieldID(class_, "__native_ref", "J");
    return reinterpret_cast<T>(env->GetLongField(object_, nativeRefField));
  }

} // namespace

::jobject Java_com_arkanosis_jpdh_JPDH_nOpenQuery(::JNIEnv* env, ::jclass, ::jstring dataSource_) {
  JNIString dataSource(env, dataSource_);
  ::PDH_HQUERY query;
  ::PDH_STATUS status = ::PdhOpenQuery(dataSource, 0, &query);
  if (status != ERROR_SUCCESS) {
    jpdh::throwException(env, status);
    return 0;
  }
  return newObject(env, "com/arkanosis/jpdh/Query", new jpdh::Query(dataSource, query));
}

::jobject Java_com_arkanosis_jpdh_Query_nAddCounter(::JNIEnv* env, ::jobject query_, ::jstring fullPath_) {
  jpdh::Query* query = getObject<jpdh::Query*>(env, query_);
  JNIString fullPath(env, fullPath_);

  if (query->hasCounter(fullPath)) {
    env->ThrowNew(env->FindClass("com/arkanosis/jpdh/DuplicateCounterException"), fullPath);
    return 0;
  }

  jpdh::PIDParser pidParser;
  if (pidParser.parse(fullPath)) {
    jpdh::ProcessCounter* counter = new jpdh::ProcessCounter(env, pidParser.getPID(), pidParser.getPrefix(), pidParser.getSuffix(), fullPath, query);
    if (!counter->isValid()) {
      delete counter;
      return 0;
    }
    query->add(counter);
    return newObject(env, "com/arkanosis/jpdh/Counter", counter);
  } else if (pidParser.error()) {
    env->ThrowNew(env->FindClass("com/arkanosis/jpdh/JPDHException"), pidParser.getErrorMessage().c_str());
    return 0;
  } else if (false) { // TODO FIXME has "\\Memory" in it
    return 0;
  } else if (false) { // TODO FIXME has "\\CpuCount" in it
    return 0;
  }
  ::PDH_HCOUNTER handle;
  ::PDH_STATUS status = ::PdhAddEnglishCounter(query->getHandle(), fullPath, 0, &handle);
  if (status != ERROR_SUCCESS) {
    jpdh::throwException(env, status);
    return 0;
  }
  jpdh::Counter* counter = new jpdh::Counter(fullPath, handle);
  query->add(counter);
  return newObject(env, "com/arkanosis/jpdh/Counter", counter);
}

void Java_com_arkanosis_jpdh_Query_removeCounter(::JNIEnv* env, ::jobject query_, ::jobject counter_) {
  jpdh::Query* query = getObject<jpdh::Query*>(env, query_);
  jpdh::Counter* counter = getObject<jpdh::Counter*>(env, counter_);
  if (!query->remove(counter)) {
    env->ThrowNew(env->FindClass("com/arkanosis/jpdh/JPDHException"), "Unable to remove counter from query");
    return;
  }
  if (counter->isValid()) {
    counter->removeHandle(env);
  }
  delete counter;
}

void Java_com_arkanosis_jpdh_Query_collectData(::JNIEnv* env, ::jobject query_) {
  jpdh::Query* query = getObject<jpdh::Query*>(env, query_);
  ::PDH_STATUS status = ::PdhCollectQueryData(query->getHandle());
  if (status != ERROR_SUCCESS) {
    jpdh::throwException(env, status);
  }
}

void Java_com_arkanosis_jpdh_Query_close(::JNIEnv* env, ::jobject query_) {
  jpdh::Query* query = getObject<jpdh::Query*>(env, query_);
  ::PDH_STATUS status = ::PdhCloseQuery(query->getHandle());
  if (status != ERROR_SUCCESS) {
    jpdh::throwException(env, status);
  }
  delete query;
}

::jdouble Java_com_arkanosis_jpdh_Counter_getDoubleValue(::JNIEnv* env, ::jobject counter_) {
  jpdh::Counter* counter = getObject<jpdh::Counter*>(env, counter_);
  DWORD type;
  ::PDH_FMT_COUNTERVALUE value;
  ::PDH_HCOUNTER handle;
  if (!counter->getHandle(env, handle)) {
    return .0;
  }
  ::PDH_STATUS status = ::PdhGetFormattedCounterValue(handle, PDH_FMT_DOUBLE, &type, &value);
  if (status != ERROR_SUCCESS) {
    jpdh::throwException(env, status);
    return .0;
  } else if (value.CStatus != PDH_CSTATUS_VALID_DATA && value.CStatus != PDH_CSTATUS_NEW_DATA) {
    jpdh::throwException(env, value.CStatus);
    return .0;
  }
  return value.doubleValue;
}

::jint Java_com_arkanosis_jpdh_Counter_getIntegerValue(::JNIEnv* env, ::jobject counter_) {
  jpdh::Counter* counter = getObject<jpdh::Counter*>(env, counter_);
  DWORD type;
  ::PDH_FMT_COUNTERVALUE value;
  ::PDH_HCOUNTER handle;
  if (!counter->getHandle(env, handle)) {
    return 0;
  }
  ::PDH_STATUS status = ::PdhGetFormattedCounterValue(handle, PDH_FMT_LONG, &type, &value);
  if (status != ERROR_SUCCESS) {
    jpdh::throwException(env, status);
    return 0;
  } else if (value.CStatus != PDH_CSTATUS_VALID_DATA && value.CStatus != PDH_CSTATUS_NEW_DATA) {
    jpdh::throwException(env, value.CStatus);
    return 0;
  }
  return value.longValue;
}

::jlong Java_com_arkanosis_jpdh_Counter_getLongValue(::JNIEnv* env, ::jobject counter_) {
  jpdh::Counter* counter = getObject<jpdh::Counter*>(env, counter_);
  DWORD type;
  ::PDH_FMT_COUNTERVALUE value;
  ::PDH_HCOUNTER handle;
  if (!counter->getHandle(env, handle)) {
    return 0;
  }
  ::PDH_STATUS status = ::PdhGetFormattedCounterValue(handle, PDH_FMT_LARGE, &type, &value);
  if (status != ERROR_SUCCESS) {
    jpdh::throwException(env, status);
    return 0;
  } else if (value.CStatus != PDH_CSTATUS_VALID_DATA && value.CStatus != PDH_CSTATUS_NEW_DATA) {
    jpdh::throwException(env, value.CStatus);
    return 0;
  }
  return value.largeValue;
}
