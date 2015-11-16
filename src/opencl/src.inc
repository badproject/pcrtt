
if USE_OPENCL

OCL_HEADER_FILE := $(top_srcdir)/src/opencl/csa_rainbow_a.cl
OCL_GEN_HEADER := $(top_builddir)/src/opencl/csa_rainbow_a.h

OPENCL_SRC += opencl/opencl.cxx

-include generator-ocl
.PHONY: generator-ocl

generator-ocl: $(OCL_GEN_HEADER)

$(OCL_GEN_HEADER):
	@echo -e "static const int kernel_code_size = \
		$$(wc -c <$(OCL_HEADER_FILE));\n" >> $@
	@echo "static const char kernel_code_src[] = " >> $@
	@sed -e 's/["\]/\\&/g' -e 's/^/    "/' -e 's/$$/\\n"/' -e '$$s/$$/;/' \
	 $(OCL_HEADER_FILE) >> $@

AM_CXXFLAGS += -I$(top_builddir)/src/opencl

CLEANFILES += $(OCL_GEN_HEADER)

endif # USE_OPENCL
