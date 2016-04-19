#include <iostream>
#include <string>

#include "com/arkanosis/jpdh/PIDParser.hpp"
#include "com/arkanosis/jpdh/ProcessParser.hpp"

namespace {

  bool assert_parse_pid(jpdh::PIDParser& parser, char const* fullPath, int pid, char const* prefix, char const* suffix) {
    if (!parser.parse(fullPath)) {
      std::cerr << "Error: could not parse '" << fullPath << "': "
                << (parser.error()
                    ? parser.getErrorMessage()
                    : "no error")
                << std::endl;
      return false;
    } else if (parser.getPID() != pid) {
      std::cerr << "Error: wrong PID while parsing '" << fullPath << "': expected " << pid
                << ", got " << parser.getPID() << std::endl;
      return false;
    } else if (parser.getPrefix() != prefix) {
      std::cerr << "Error: wrong prefix while parsing '" << fullPath << "': expected '" << prefix
                << "', got '" << parser.getPrefix() << "'" << std::endl;
      return false;
    } else if (parser.getSuffix() != suffix) {
      std::cerr << "Error: wrong suffix while parsing '" << fullPath << "': expected '" << suffix
                << "', got '" << parser.getSuffix() << "'" << std::endl;
      return false;
    }
    return true;
  }

  bool assert_parse_process(jpdh::ProcessParser& parser, char const* fullPath, char const* process) {
    if (!parser.parse(fullPath)) {
      std::cerr << "Error: could not parse '" << fullPath << "': "
                << (parser.error()
                    ? parser.getErrorMessage()
                    : "no error")
                << std::endl;
      return false;
    } else if (parser.getProcess() != process) {
      std::cerr << "Error: wrong Process while parsing '" << fullPath << "': expected " << process
                << ", got " << parser.getProcess() << std::endl;
      return false;
    }
    return true;
  }

  template <typename ParserT>
  bool assert_not_parse(ParserT& parser, char const* fullPath, bool error, char const* errorMessage = nullptr) {
    if (parser.parse(fullPath)) {
      std::cerr << "Error: could parse '" << fullPath << "': not expected to parse" << std::endl;
      return false;
    } else if (parser.error() != error) {
      std::cerr << "Error: while parsing '" << fullPath << "': "
                << (parser.error()
                    ? "unexpected parse error"
                    : "expected parse error")
                << std::endl;
      return false;
    } else if (error &&
               parser.getErrorMessage() != errorMessage) {
      std::cerr << "Error: wrong error message while parsing '" << fullPath << "': expected '" << errorMessage
                << "', got '" << parser.getErrorMessage() << "'"
                << std::endl;
      return false;
    }
    return true;
  }

  bool test_pid_parser() {
    jpdh::PIDParser parser;
    return (
      assert_parse_pid(parser, R"=(\\foo\PID(42)\myCount)=", 42, R"(\\foo\)", R"(\myCount)") &&
      assert_parse_pid(parser, R"=(\PID(42)\myCount)=", 42, R"(\)", R"(\myCount)") &&
      assert_not_parse(parser, R"=(\\foo\Process(test)\blah)=", false) &&
      assert_not_parse(parser, R"=(\\foo\PID=)=", false) &&
      assert_not_parse(parser, R"=(\\foo\PI=)=", false) &&
      assert_not_parse(parser, R"=(\\foo\PID(test)\blah)=", true, "Invalid digit 't' in PID in expression 'PID(t'") &&
      assert_not_parse(parser, R"=(\\foo\PID()\blah)=", true, "Invalid PID in expression 'PID()'") &&
      assert_not_parse(parser, R"=(\\foo\PID()=", true, "Unexpected end of PID in expression 'PID('")
    );
  }

  bool test_process_parser() {
    jpdh::ProcessParser parser;
    return (
      assert_parse_process(parser, R"=(\\foo\Process(blah#21)\myCount)=", "blah#21") &&
      assert_parse_process(parser, R"=(\Process(bim)\myCount)=", "bim") &&
      assert_not_parse(parser, R"=(\\foo\PID(32)\blah)=", false) &&
      assert_not_parse(parser, R"=(\\foo\Process=)=", false) &&
      assert_not_parse(parser, R"=(\\foo\Pro=)=", false) &&
      assert_not_parse(parser, R"=(\\foo\Process()\blah)=", true, "Invalid process name in expression 'Process()'") &&
      assert_not_parse(parser, R"=(\\foo\Process()=", true, "Unexpected end of process in expression 'Process('")
    );
  }

} // namespace

int main(int argc, char const* argv[]) {
  return !(
    test_pid_parser() &&
    test_process_parser()
  );
}
