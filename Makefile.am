AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = -I m4
AM_CXXFLAGS = -I $(srcdir)/stree -Wall

HEADER_FILES = \
	stree/stree.hpp \
	stree/environment.hpp \
	stree/eval.hpp \
	stree/macros.hpp \
	stree/parser.hpp \
	stree/stats.hpp \
	stree/string.hpp \
	stree/tree.hpp
SOURCE_FILES = \
	$(HEADER_FILES) \
	src/environment.cpp \
	src/eval.cpp \
	src/parser.cpp \
	src/stats.cpp \
	src/string.cpp \
	src/tree.cpp

lib_LTLIBRARIES = libstree.la
libstree_ladir = $(includedir)
nobase_libstree_la_HEADERS = $(HEADER_FILES)
libstree_la_SOURCES = $(SOURCE_FILES)

TESTS = test_string1 \
	test_grow1 \
	test_tree_size1 \
	test_tree_nth_node1 \
	test_tree_nth_node2 \
	test_copy1 \
	test_subtree_string1 \
	test_node_manager_stats1 \
	test_swap1 \
	test_mutate1
check_PROGRAMS = $(TESTS)

test_string1_SOURCES = tests/test_string1.cpp
test_string1_LDADD = libstree.la

test_grow1_SOURCES = tests/test_grow1.cpp
test_grow1_LDADD = libstree.la

test_tree_size1_SOURCES = tests/test_tree_size1.cpp tests/macros.hpp
test_tree_size1_LDADD = libstree.la

test_tree_nth_node1_SOURCES = tests/test_tree_nth_node1.cpp tests/macros.hpp
test_tree_nth_node1_LDADD = libstree.la

test_tree_nth_node2_SOURCES = tests/test_tree_nth_node2.cpp tests/macros.hpp
test_tree_nth_node2_LDADD = libstree.la

test_copy1_SOURCES = tests/test_copy1.cpp tests/macros.hpp
test_copy1_LDADD = libstree.la

test_subtree_string1_SOURCES = tests/test_subtree_string1.cpp tests/macros.hpp
test_subtree_string1_LDADD = libstree.la

test_node_manager_stats1_SOURCES = tests/test_node_manager_stats1.cpp tests/macros.hpp
test_node_manager_stats1_LDADD = libstree.la

test_swap1_SOURCES = tests/test_swap1.cpp tests/macros.hpp
test_swap1_LDADD = libstree.la

test_mutate1_SOURCES = tests/test_mutate1.cpp tests/macros.hpp
test_mutate1_LDADD = libstree.la
