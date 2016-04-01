-include windows.mk
JAVA ?= java.exe
JAVAH ?= javah.exe
JAVAC ?= javac.exe
JAR ?= jar.exe
CXX ?= cl.exe
LD ?= link.exe
CXXFLAGS ?= -nologo
IFLAGS ?=
LDFLAGS ?= -nologo

JPDH=com/arkanosis/jpdh

JAVASRC= \
	$(JPDH)/JPDH.java \
	$(JPDH)/JPDHException.java \
	$(JPDH)/Query.java \
	$(JPDH)/Counter.java
JAVACLASS=$(patsubst %.java,%.class,$(JAVASRC))

.PHONY: all test clean

all: jpdh.jar

$(JPDH)/jnijpdh.obj: $(JPDH)/jpdh.cpp $(JPDH)/jpdh.h
	$(CXX) -c $(CXXFLAGS) -I. $(IFLAGS) $< -Fo$@

$(JPDH)/jpdh.h: $(JAVASRC)
	$(JAVAH) -o $@ com.arkanosis.jpdh.JPDH com.arkanosis.jpdh.Query com.arkanosis.jpdh.Counter
	touch $@

%.class: %.java
	$(JAVAC) $<

jnijpdh.dll: $(JPDH)/jnijpdh.obj
	$(LD) $(LDFLAGS) -dll -out:$@ Pdh.Lib $^

jpdh.jar: $(JPDH)/Manifest.txt $(JAVACLASS) jnijpdh.dll
	$(JAR) cfm $@ $^

test: jpdh.jar
	$(JAVA) -jar jpdh.jar

clean:
	rm -f \
		$(JPDH)/*.class \
		$(JPDH)/jpdh.h \
		$(JPDH)/jnijpdh.obj \
		jnijpdh.dll \
		jnijpdh.exp \
		jnijpdh.lib \
		jpdh.jar
