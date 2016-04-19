# JPDH — Windows Performance Data Helper for Java

JPDH is a Java library to retrieve performance counter values on Microsoft
Windows using the PDH (Performance Data Helper) API and some additional features built on top of it.

## Build

Run `make`.

If you don't have a POSIX environment under Windows, you might want to have
a look at [MSYS2](https://msys2.github.io/).

Additionally, you may run the test suite using `make runtest`.

## Installation

Add jpdh.jar in your CLASSPATH.

## Usage

    import com.arkanosis.jpdh.Counter;
    import com.arkanosis.jpdh.JPDH;
    import com.arkanosis.jpdh.Query;

    try (Query query = JPDH.openQuery()) {
        Counter diskCounter = query.addCounter("\\PhysicalDisk(_Total)\\Disk Read Bytes/sec");
        Counter cpuCounter = query.addCounter("\\PID(8584)\\% User Time");
        query.collectData();
        System.out.println(diskCounter.getDoubleValue());
        System.out.println(cpuCounter.getIntegerValue());
        query.removeCounter(diskCounter);
    }

Any valid PDH counter path can be used, and additional counter path using the non-PDH “PID” object can be used as well:

    \\computer\Process(parent/instance#index)\counter
    \\computer\PID(pid)\counter

## History

* 2016-04-19: version 1.1 — Additional “PID” object which maps to the “Process” object for the right process instance and index
* 2016-04-01: version 1.0 — Initial version exposing PDH features in Java

## Contact

Send an email to Jérémie Roquet <jroquet@arkanosis.net>.

## License

JPDH is distributed under the BSD license, see the LICENSE file.
