#ifndef JPDH_UTILS_HPP_
# define JPDH_UTILS_HPP_

# include <jni.h>
# include <Pdh.h>

namespace jpdh {

  void throwException(::JNIEnv* env, ::PDH_STATUS status, const char* file, long line);

} // namespace jpdh

#define THROW_EXCEPTION(ENV, STATUS) jpdh::throwException(ENV, STATUS, __FILE__, __LINE__)

#endif // !JPDH_UTILS_HPP_
