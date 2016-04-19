-include ../windows.mk
JAVA ?= java.exe
JAVAH ?= javah.exe
JAVAC ?= javac.exe
JAR ?= jar.exe
CXX ?= cl.exe
LD ?= link.exe
CXXFLAGS ?= -nologo -D_HAS_EXCEPTIONS=0
IFLAGS ?=
LDFLAGS ?= -nologo

JPDH=com/arkanosis/jpdh

CPPSRC = \
	$(JPDH)/jpdh.cpp \
	$(JPDH)/utils.cpp \
	$(JPDH)/Counter.cpp \
	$(JPDH)/PIDParser.cpp \
	$(JPDH)/ProcessParser.cpp \
	$(JPDH)/Query.cpp
CPPOBJ = $(patsubst %.cpp,%.obj,$(CPPSRC))
JAVASRC = \
	$(JPDH)/DuplicateCounterException.java \
	$(JPDH)/JPDH.java \
	$(JPDH)/JPDHException.java \
	$(JPDH)/NoSuchPIDException.java \
	$(JPDH)/Query.java \
	$(JPDH)/Counter.java
JAVACLASS = $(patsubst %.java,%.class,$(JAVASRC))

.PHONY: all sign test clean

all: jpdh.jar

%.obj: %.cpp
	$(CXX) -c $(CXXFLAGS) -I. $(IFLAGS) $< -Fo$@

$(JPDH)/jpdh.obj: $(JPDH)/jpdh.h $(JPDH)/utils.hpp $(JPDH)/Counter.hpp $(JPDH)/PIDParser.hpp $(JPDH)/Query.hpp
$(JPDH)/utils.obj: $(JPDH)/utils.hpp
$(JPDH)/Counter.obj: $(JPDH)/Counter.hpp $(JPDH)/ProcessParser.hpp $(JPDH)/Query.hpp $(JPDH)/utils.hpp
$(JPDH)/PIDParser.obj: $(JPDH)/PIDParser.hpp
$(JPDH)/ProcessParser.obj: $(JPDH)/ProcessParser.hpp
$(JPDH)/Query.obj: $(JPDH)/Counter.hpp $(JPDH)/Query.hpp

$(JPDH)/jpdh.h: $(JAVASRC)
	$(JAVAH) -o $@ com.arkanosis.jpdh.JPDH com.arkanosis.jpdh.Query com.arkanosis.jpdh.Counter
	touch $@

%.class: %.java
	$(JAVAC) $<

jnijpdh.dll: $(CPPOBJ)
	$(LD) $(LDFLAGS) -dll -out:$@ Pdh.Lib $^

jpdh.jar: $(JPDH)/Manifest.txt $(JAVACLASS) jnijpdh.dll
	$(JAR) cfm $@ $^

jpdh.jar.asc: jpdh.jar
	gpg --default-key jroquet@arkanosis.net --output $@ --detach-sig $<

test: jpdh.jar
	$(JAVA) -jar jpdh.jar

clean:
	rm -f \
		*.log \
		*.mdmp \
		$(JPDH)/*.class \
		$(JPDH)/*.obj \
		$(JPDH)/jpdh.h \
		jnijpdh.dll \
		jnijpdh.exp \
		jnijpdh.lib \
		jpdh.jar
