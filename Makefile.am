AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = -I m4
AM_CXXFLAGS = -I $(srcdir)/stree -Wall

HEADER_FILES = \
	stree/eval.hpp \
	stree/macro.hpp \
	stree/tree.hpp
SOURCE_FILES = \
	$(HEADER_FILES) \
	src/main.cpp \
	src/eval.cpp \
	src/tree.cpp

# lib_LTLIBRARIES = libstree.la
# libstree_ladir = $(includedir)
# nobase_libstree_la_HEADERS = $(HEADER_FILES)
# libstree_la_SOURCES = $(SOURCE_FILES)

bin_PROGRAMS = stree
stree_SOURCES = $(SOURCE_FILES)

