
if USE_CUDA

SUFFIXES=.cu

CUDA_INC = cuda/cuda_dvbcsa.h		\
			cuda/cuda_dvbcsa_pv.h	\
			cuda/cuda.h				\
			cuda/include.h

CUDA_SRC += cuda/cuda.cu $(CUDA_INC)

# include the dependency tracking files
include @top_builddir@/src/cuda/$(DEPDIR)/cuda.Plo

LOFILES = $(CUDA_SRC:.cu=.lo)

cuda_verbose = $(cuda_verbose_@AM_V@)
cuda_verbose_ = $(cuda_verbose_@AM_DEFAULT_V@)
cuda_verbose_0 = @echo "  NVCC     $@"

$(LOFILES): %.lo: %.cu
	$(cuda_verbose)depbase=`echo $@ | sed 's|[^/]*$$|$(DEPDIR)/&|;s|\.lo$$||'`;\
	$(NVCC) $(NVCC_FLAGS) $(DEFAULT_INCLUDES) $(CPPFLAGS) $(INCLUDES) $(DEFS) \
	  -M -MT $@ -o $$depbase.Plo $<; \
	$(LIBTOOL) $(AM_V_lt) --tag=CUDA $(AM_LIBTOOLFLAGS) $(LIBTOOLFLAGS) \
		--mode=compile $(NVCC) $(DEFS) \
		$(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) \
		$(AM_NVCCFLAGS) $(NVCC_FLAGS) -c -o $@ $<

libpcrtt_la_LIBADD = $(LOFILES)
EXTRA_DIST += $(CUDA_INC)
CLEANFILES += $(LOFILES)

endif # USE_CUDA
