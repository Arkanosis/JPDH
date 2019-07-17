#include <sstream>

#include "com/arkanosis/jpdh/utils.hpp"

namespace jpdh {

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
      std::stringstream fullMessage;
      fullMessage << file << ":" << line << ": " << message;
      env->ThrowNew(env->FindClass("com/arkanosis/jpdh/JPDHException"), fullMessage.str().c_str());
      ::LocalFree(message);
    } else {
      env->ThrowNew(env->FindClass("com/arkanosis/jpdh/JPDHException"), "Unable to get Pdh.Dll module handle");
    }
  }

} // namespace jpdh
