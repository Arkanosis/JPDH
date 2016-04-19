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
	$(JPDH)/test_parser.cpp
CPPEXE = $(patsubst %.cpp,%.exe,$(CPPSRC))
JAVASRC = \
	$(JPDH)/JPDHTest.java
JAVACLASS = $(patsubst %.java,%.class,$(JAVASRC))

.PHONY: all run clean

all: $(CPPEXE) $(JAVACLASS)

%.exe: %.cpp
	$(CXX) $(CXXFLAGS) -I. -I../src $(IFLAGS) $^ -Fe$@ -link $(LDFLAGS)

$(JPDH)/test_parser.exe: ../src/$(JPDH)/PIDParser.cpp ../src/$(JPDH)/ProcessParser.cpp

%.class: %.java
	$(JAVAC) -cp '../src/*' $<

run: all
	$(CPPEXE)
	$(JAVA) -cp '../src/*:.' com.arkanosis.jpdh.JPDHTest

clean:
	rm -f \
		*.exe \
		*.log \
		*.mdmp \
		*.obj \
		$(JPDH)/*.class
