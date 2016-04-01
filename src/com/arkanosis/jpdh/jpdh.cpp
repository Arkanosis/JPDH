#include <jni.h>
#include <Pdh.h>
#include <PdhMsg.h>

#include "com/arkanosis/jpdh/jpdh.h"

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

  void throwException(::JNIEnv* env, ::PDH_STATUS status) {
    HMODULE handle = ::GetModuleHandle("Pdh.Dll");
    if (handle) {
      char* message;
      ::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                      FORMAT_MESSAGE_FROM_HMODULE,
                      handle,
                      status,
                      0,
                      reinterpret_cast<LPTSTR>(&message),
                      0,
                      nullptr);
      env->ThrowNew(env->FindClass("com/arkanosis/jpdh/JPDHException"), message);
      ::LocalFree(message);
    } else {
      env->ThrowNew(env->FindClass("com/arkanosis/jpdh/JPDHException"), "Unable to get Pdh.Dll module handle");
    }
  }

} // namespace

::jobject Java_com_arkanosis_jpdh_JPDH_openQuery(::JNIEnv* env, ::jclass, ::jstring dataSource_) {
  JNIString dataSource(env, dataSource_);
  ::PDH_HQUERY query;
  ::PDH_STATUS status = ::PdhOpenQuery(dataSource, 0, &query);
  if (status != ERROR_SUCCESS) {
    throwException(env, status);
    return 0;
  }
  return newObject(env, "com/arkanosis/jpdh/Query", query);
}

::jobject Java_com_arkanosis_jpdh_Query_addCounter(::JNIEnv* env, ::jobject query_, ::jstring fullCounterPath_) {
  JNIString fullCounterPath(env, fullCounterPath_);
  ::PDH_HCOUNTER counter;
  ::PDH_STATUS status = ::PdhAddCounter(getObject<::PDH_HQUERY>(env, query_), fullCounterPath, 0, &counter);
  if (status != ERROR_SUCCESS) {
    throwException(env, status);
    return 0;
  }
  return newObject(env, "com/arkanosis/jpdh/Counter", counter);
}

void Java_com_arkanosis_jpdh_Query_collectData(::JNIEnv* env, ::jobject query_) {
  ::PDH_STATUS status = ::PdhCollectQueryData(getObject<::PDH_HQUERY>(env, query_));
  if (status != ERROR_SUCCESS) {
    throwException(env, status);
  }
}

void Java_com_arkanosis_jpdh_Query_close(::JNIEnv* env, ::jobject query_) {
  ::PDH_STATUS status = ::PdhCloseQuery(getObject<::PDH_HQUERY>(env, query_));
  if (status != ERROR_SUCCESS) {
    throwException(env, status);
  }
}

::jdouble Java_com_arkanosis_jpdh_Counter_getDoubleValue(::JNIEnv* env, ::jobject counter_) {
  DWORD type;
  ::PDH_FMT_COUNTERVALUE value;
  ::PDH_STATUS status = ::PdhGetFormattedCounterValue(getObject<::PDH_HCOUNTER>(env, counter_), PDH_FMT_DOUBLE, &type, &value);
  if (status != ERROR_SUCCESS) {
    throwException(env, status);
    return .0;
  } else if (value.CStatus != PDH_CSTATUS_VALID_DATA && value.CStatus != PDH_CSTATUS_NEW_DATA) {
    throwException(env, value.CStatus);
    return .0;
  }
  return value.doubleValue;
}

::jint Java_com_arkanosis_jpdh_Counter_getIntegerValue(::JNIEnv* env, ::jobject counter_) {
  DWORD type;
  ::PDH_FMT_COUNTERVALUE value;
  ::PDH_STATUS status = ::PdhGetFormattedCounterValue(getObject<::PDH_HCOUNTER>(env, counter_), PDH_FMT_LONG, &type, &value);
  if (status != ERROR_SUCCESS) {
    throwException(env, status);
    return 0;
  } else if (value.CStatus != PDH_CSTATUS_VALID_DATA && value.CStatus != PDH_CSTATUS_NEW_DATA) {
    throwException(env, value.CStatus);
    return 0;
  }
  return value.longValue;
}

::jlong Java_com_arkanosis_jpdh_Counter_getLongValue(::JNIEnv* env, ::jobject counter_) {
  DWORD type;
  ::PDH_FMT_COUNTERVALUE value;
  ::PDH_STATUS status = ::PdhGetFormattedCounterValue(getObject<::PDH_HCOUNTER>(env, counter_), PDH_FMT_LARGE, &type, &value);
  if (status != ERROR_SUCCESS) {
    throwException(env, status);
    return 0;
  } else if (value.CStatus != PDH_CSTATUS_VALID_DATA && value.CStatus != PDH_CSTATUS_NEW_DATA) {
    throwException(env, value.CStatus);
    return 0;
  }
  return value.largeValue;
}

void Java_com_arkanosis_jpdh_Counter_remove(::JNIEnv* env, ::jobject counter_) {
  ::PDH_STATUS status = ::PdhRemoveCounter(getObject<::PDH_HCOUNTER>(env, counter_));
  if (status != ERROR_SUCCESS) {
    throwException(env, status);
  }
}
