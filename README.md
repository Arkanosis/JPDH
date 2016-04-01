# JPDH — Windows Performance Data Helper for Java

JPDH is a Java library to retrieve performance counter values on Microsoft
Windows using the PDH (Performance Data Helper) API.

## Build

Run `make` in the `src/` directory.

If you don't have a POSIX environment under Windows, you might want to have
a look at [MSYS2](https://msys2.github.io/).

## Installation

Add jpdh.jar in your CLASSPATH.

## Usage

    import com.arkanosis.jpdh.Counter;
    import com.arkanosis.jpdh.JPDH;
    import com.arkanosis.jpdh.Query;

    try (Query query = JPDH.openQuery()) {
        Counter counter = query.addCounter("\\Processor(_Total)\\% Processor Time");
        query.collectData();
        System.out.println(counter.getDoubleValue());
		counter.remove();
    }

## History

* 2016-04-01: version 1.0

## Contact

Send an email to Jérémie Roquet <jroquet@arkanosis.net>.

## License

JPDH is distributed under the BSD license, see the LICENSE file.
