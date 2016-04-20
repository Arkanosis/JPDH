#include "com/arkanosis/jpdh/utils.hpp"

namespace jpdh {

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

} // namespace jpdh
