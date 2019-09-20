# JPDH — Windows Performance Data Helper for Java [![Maven Central](https://img.shields.io/maven-central/v/com.arkanosis/jpdh.svg?label=Maven%20Central)](https://search.maven.org/search?q=g:%22com.arkanosis%22%20AND%20a:%22jpdh%22) [![License](https://img.shields.io/badge/license-BSD-blue.svg)](/LICENSE)

JPDH is a Java library to retrieve performance counter values on Microsoft
Windows using the PDH (Performance Data Helper) API and some additional features built on top of it.

## Installation

### Manually

Add jpdh.jar to your CLASSPATH.

### With Maven

```xml
<dependency>
  <groupId>com.arkanosis</groupId>
  <artifactId>jpdh</artifactId>
  <version>1.1.4</version>
</dependency>
```

### With Gradle

```kotlin
compile("com.arkanosis:jpdh:1.1.4")
```

## Usage

```java
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
```

Any valid PDH counter path can be used, and additional counter path using the non-PDH “PID” object can be used as well:

```
\\computer\Process(parent/instance#index)\counter
\\computer\PID(pid)\counter
```

## Build

Run `make`.

If you don't have a POSIX environment under Windows, you might want to have
a look at [MSYS2](https://msys2.github.io/).

Additionally, you may run the test suite using `make runtest`.

## History

* 2019-09-20: version 1.1.4 — Bugfix release. Counters based on the virtual “PID” object sometimes couldn't be initialized while lots of processes were started of stopped.
* 2017-09-14: version 1.1.3 — Bugfix release. The virtual “PID” object didn't work on localized (eg. German) versions of Windows.
* 2016-06-08: version 1.1.2 — Bugfix release. Calling Query.removeCounter(counter) on counters associated with dead processes was throwing an exception.
* 2016-06-01: version 1.1.1 — Bugfix release. Calling Query.removeCounter(counter) on counters associated with dead processes was throwing an exception.
* 2016-04-19: version 1.1 — Additional “PID” object which maps to the “Process” object for the right process instance and index
* 2016-04-01: version 1.0 — Initial version exposing PDH features in Java

## Contact

Send an email to Jérémie Roquet <jroquet@arkanosis.net>.

## License

JPDH is distributed under the BSD license, see the LICENSE file.
