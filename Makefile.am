AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = -I m4
AM_CXXFLAGS = -I $(srcdir)/stree -Wall

HEADER_FILES = \
	stree/environment.hpp \
	stree/eval.hpp \
	stree/macro.hpp \
	stree/parser.hpp \
	stree/string.hpp \
	stree/tree.hpp
SOURCE_FILES = \
	$(HEADER_FILES) \
	src/main.cpp \
	src/environment.cpp \
	src/eval.cpp \
	src/parser.cpp \
	src/string.cpp \
	src/tree.cpp

# lib_LTLIBRARIES = libstree.la
# libstree_ladir = $(includedir)
# nobase_libstree_la_HEADERS = $(HEADER_FILES)
# libstree_la_SOURCES = $(SOURCE_FILES)

bin_PROGRAMS = stree
stree_SOURCES = $(SOURCE_FILES)

