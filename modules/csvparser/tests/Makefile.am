modules_csvparser_tests_TESTS			=	\
	modules/csvparser/tests/test_csvparser		\
	modules/csvparser/tests/test_csvparser_from_config \
	modules/csvparser/tests/test_csvparser_perf \
	modules/csvparser/tests/test_filterx_func_parse_csv \
	modules/csvparser/tests/test_filterx_func_format_csv

check_PROGRAMS					+=	\
	${modules_csvparser_tests_TESTS}

EXTRA_DIST += modules/csvparser/tests/CMakeLists.txt \
	modules/basicfuncs/list-funcs.c			\
	modules/basicfuncs/tf-template.c		\
	modules/basicfuncs/tests/CMakeLists.txt

modules_csvparser_tests_test_csvparser_CFLAGS	=	\
	$(TEST_CFLAGS) -I$(top_srcdir)/modules/csvparser
modules_csvparser_tests_test_csvparser_LDADD	=	\
	$(TEST_LDADD)					\
	-dlpreopen $(top_builddir)/modules/csvparser/libcsvparser.la

modules_csvparser_tests_test_csvparser_from_config_CFLAGS	=	\
	$(TEST_CFLAGS) -I$(top_srcdir)/modules/csvparser
modules_csvparser_tests_test_csvparser_from_config_LDADD	=	\
	$(TEST_LDADD)					\
	$(PREOPEN_SYSLOGFORMAT) $(PREOPEN_BASICFUNCS) \
	-dlpreopen $(top_builddir)/modules/csvparser/libcsvparser.la

modules_csvparser_tests_test_csvparser_perf_CFLAGS	=	\
	$(TEST_CFLAGS) -I$(top_srcdir)/modules/csvparser
modules_csvparser_tests_test_csvparser_perf_LDADD	=	\
	$(TEST_LDADD)					\
	-dlpreopen $(top_builddir)/modules/csvparser/libcsvparser.la

modules_csvparser_tests_test_filterx_func_parse_csv_CFLAGS	=	\
	$(TEST_CFLAGS) -I$(top_srcdir)/modules/csvparser
modules_csvparser_tests_test_filterx_func_parse_csv_LDADD	=	\
	$(TEST_LDADD)					\
	-dlpreopen $(top_builddir)/modules/csvparser/libcsvparser.la

modules_csvparser_tests_test_filterx_func_format_csv_CFLAGS	=	\
	$(TEST_CFLAGS) -I$(top_srcdir)/modules/csvparser
modules_csvparser_tests_test_filterx_func_format_csv_LDADD	=	\
	$(TEST_LDADD)					\
	-dlpreopen $(top_builddir)/modules/csvparser/libcsvparser.la
