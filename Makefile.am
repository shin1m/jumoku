EXTRA_DIST = \
	tree.h \
	array.h \
	test_array.h

CLEANFILES = *.gcda *.gcno *.gcov

AM_CPPFLAGS =
AM_CXXFLAGS = -fprofile-arcs -ftest-coverage
AM_LDFLAGS =
if DEBUG
AM_CXXFLAGS += -g
else
AM_CPPFLAGS += -DNDEBUG
AM_CXXFLAGS += -O3
endif
if PROFILE
AM_CXXFLAGS += -pg
AM_LDFLAGS += -pg
endif
AM_DEFAULT_SOURCE_EXT = .cc
check_PROGRAMS = test_array test_array_insert
TESTS = $(check_PROGRAMS)
