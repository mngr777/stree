AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = -I m4
AM_CXXFLAGS = -I $(srcdir)/stree -Wall

NODE_PACKED_HEADER_FILES = \
	stree/node/packed.hpp \
	stree/node/packed/id.hpp \
	stree/node/packed/manager.hpp \
	stree/node/packed/stats.hpp
NODE_PACKED_SOURCE_ONLY_FILES =  \
	src/node/packed/id.cpp \
	src/node/packed/manager.cpp \
	src/node/packed/stats.cpp

NODE_HEADER_FILES = $(NODE_PACKED_HEADER_FILES)
NODE_SOURCE_ONLY_FILES = $(NODE_PACKED_SOURCE_ONLY_FILES)

HEADER_FILES = \
	$(NODE_HEADER_FILES) \
	stree/stree.hpp \
	stree/algorithm.hpp \
	stree/compare.hpp \
	stree/environment.hpp \
	stree/eval.hpp \
	stree/exec.hpp \
	stree/macros.hpp \
	stree/node.hpp \
	stree/parser.hpp \
	stree/search.hpp \
	stree/string.hpp \
	stree/tree.hpp \
	stree/types.hpp
SOURCE_FILES = \
	$(HEADER_FILES) \
	$(NODE_SOURCE_ONLY_FILES) \
	src/algorithm.cpp \
	src/compare.cpp \
	src/environment.cpp \
	src/eval.cpp \
	src/exec.cpp \
	src/parser.cpp \
	src/search.cpp \
	src/string.cpp \
	src/tree.cpp \
	src/types.cpp

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
	test_mutate1 \
	test_move1 \
	test_copy2 \
	test_common_region1 \
	test_select1

check_PROGRAMS = $(TESTS)

TEST_SOURCES = tests/macros.hpp
TEST_LIBS = libstree.la

test_string1_SOURCES = tests/test_string1.cpp
test_string1_LDADD = libstree.la

test_grow1_SOURCES = tests/test_grow1.cpp
test_grow1_LDADD = libstree.la

test_tree_size1_SOURCES = tests/test_tree_size1.cpp $(TEST_SOURCES)
test_tree_size1_LDADD = libstree.la

test_tree_nth_node1_SOURCES = tests/test_tree_nth_node1.cpp $(TEST_SOURCES)
test_tree_nth_node1_LDADD = libstree.la

test_tree_nth_node2_SOURCES = tests/test_tree_nth_node2.cpp $(TEST_SOURCES)
test_tree_nth_node2_LDADD = libstree.la

test_copy1_SOURCES = tests/test_copy1.cpp $(TEST_SOURCES)
test_copy1_LDADD = libstree.la

test_subtree_string1_SOURCES = tests/test_subtree_string1.cpp $(TEST_SOURCES)
test_subtree_string1_LDADD = libstree.la

test_node_manager_stats1_SOURCES = tests/test_node_manager_stats1.cpp $(TEST_SOURCES) \
	tests/node_manager_macros.hpp
test_node_manager_stats1_LDADD = libstree.la

test_swap1_SOURCES = tests/test_swap1.cpp $(TEST_SOURCES)
test_swap1_LDADD = libstree.la

test_mutate1_SOURCES = tests/test_mutate1.cpp $(TEST_SOURCES)
test_mutate1_LDADD = libstree.la

test_move1_SOURCES = tests/test_move1.cpp $(TEST_SOURCES) \
	tests/node_manager_macros.hpp
test_move1_LDADD = libstree.la

test_copy2_SOURCES = tests/test_copy2.cpp $(TEST_SOURCES) \
	tests/node_manager_macros.hpp
test_copy2_LDADD = libstree.la

test_common_region1_SOURCES = tests/test_common_region1.cpp $(TEST_SOURCES)
test_common_region1_LDADD = libstree.la

test_select1_SOURCES = tests/test_select1.cpp $(TEST_SOURCES)
test_select1_LDADD = $(TEST_LIBS)
