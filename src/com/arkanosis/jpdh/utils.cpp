#include <sstream>

#include "com/arkanosis/jpdh/utils.hpp"

namespace jpdh {

  void throwException(const char* exception, ::JNIEnv* env, const char* message, const char* file, long line) {
    std::stringstream ss;
    ss << file << ":" << line << ": " << message;
    env->ThrowNew(env->FindClass(exception), ss.str().c_str());
  }

  void throwException(::JNIEnv* env, ::PDH_STATUS status, const char* file, long line) {
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
      throwException("com/arkanosis/jpdh/JPDHException", env, message, file, line);
      ::LocalFree(message);
    } else {
      throwException("com/arkanosis/jpdh/JPDHException", env, "Unable to get Pdh.Dll module handle", file, line);
    }
  }

} // namespace jpdh
