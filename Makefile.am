AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = -I m4
AM_CXXFLAGS = -I $(srcdir)/stree -Wall

HEADER_FILES = \
	stree/environment.hpp \
	stree/eval.hpp \
	stree/macros.hpp \
	stree/parser.hpp \
	stree/string.hpp \
	stree/tree.hpp
SOURCE_FILES = \
	$(HEADER_FILES) \
	src/environment.cpp \
	src/eval.cpp \
	src/parser.cpp \
	src/string.cpp \
	src/tree.cpp

lib_LTLIBRARIES = libstree.la
libstree_ladir = $(includedir)
nobase_libstree_la_HEADERS = $(HEADER_FILES)
libstree_la_SOURCES = $(SOURCE_FILES)

TESTS = test_string1\
	test_grow1\
	test_tree_size1\
	test_tree_nth_node1
check_PROGRAMS = $(TESTS)
test_string1_SOURCES = tests/test_string1.cpp
test_string1_LDADD = libstree.la
test_grow1_SOURCES = tests/test_grow1.cpp
test_grow1_LDADD = libstree.la
test_tree_size1_SOURCES = tests/test_tree_size1.cpp
test_tree_size1_LDADD = libstree.la
test_tree_nth_node1_SOURCES = tests/test_tree_nth_node1.cpp
test_tree_nth_node1_LDADD = libstree.la
