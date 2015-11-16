
DVBCSA_SRC += dvbcsa/dvbcsa_algo.c				\
			dvbcsa/dvbcsa_block.c				\
			dvbcsa/dvbcsa_stream.c				\
			dvbcsa/dvbcsa_key.c					\
			dvbcsa/dvbcsa_bs_algo.c				\
			dvbcsa/dvbcsa_bs_block.c			\
			dvbcsa/dvbcsa_bs_stream.c			\
			dvbcsa/dvbcsa_bs_key.c				\
			dvbcsa/dvbcsa_bs_transpose_block.c	\
			dvbcsa/dvbcsa_bs_stream_kernel.inc	\
			dvbcsa/dvbcsa_bs_stream_kernel.h	\
			dvbcsa/dvbcsa_bs.h					\
			dvbcsa/dvbcsa_pv.h					\
			dvbcsa/dvbcsa_bs_transpose.h		\
			dvbcsa/dvbcsa_bs_uint32.h			\
			dvbcsa/dvbcsa_bs_uint64.h			\
			dvbcsa/dvbcsa_bs_mmx.h				\
			dvbcsa/dvbcsa_bs_sse.h				\
			dvbcsa/dvbcsa_bs_altivec.h			\
			dvbcsa/dvbcsa_bs_neon.h

if TRANSPOSE_32
DVBCSA_SRC += dvbcsa/dvbcsa_bs_transpose_stream32.c
else
DVBCSA_SRC += dvbcsa/dvbcsa_bs_transpose_stream.c
endif
