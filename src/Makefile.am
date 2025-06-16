INCLUDES = -I$(VARNISHSRC)/include -I$(VARNISHSRC)

vmoddir = $(VMODDIR)


vmod_LTLIBRARIES = libvmod_parsereq.la

libvmod_parsereq_la_SOURCES = \
	vmod_parsereq.c \
    vmod_parsereq_core.c

nodist_libvmod_parsereq_la_SOURCES = \
	vcc_parsereq_if.c \
	vcc_parsereq_if.h

dist_man_MANS = vmod_parsereq.3

libvmod_parsereq_la_LDFLAGS = -module -export-dynamic -avoid-version

@BUILD_VMOD_PARSEREQ@

AM_TESTS_ENVIRONMENT = \
	PATH="$(VMOD_TEST_PATH)" \
	LD_LIBRARY_PATH="$(VARNISH_LIBRARY_PATH)"
TEST_EXTENSIONS = .vtc
VTC_LOG_COMPILER = varnishtest -v
AM_VTC_LOG_FLAGS = -Dvmod_parsereq="$(VMOD_PARSEREQ)"

TESTS = @VMOD_TESTS@

EXTRA_DIST = \
	vmod_parsereq.vcc \
	$(VMOD_TESTS)
