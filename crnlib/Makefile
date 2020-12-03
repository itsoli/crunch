BUILD ?= release

CXXFLAGS += -std=c++14 -fvisibility=hidden -Wall -Werror -Wno-unused-value -Wno-unused -Wno-unused-value -Wno-unused

ifeq ($(BUILD),release)
CXXFLAGS += -O3 -flto -fomit-frame-pointer -ffast-math -fno-math-errno -fno-strict-aliasing
else
ifeq ($(BUILD),debug)
CXXFLAGS += -O0 -g -D_DEBUG
else
$(error Unsupported build, use BUILD=release/debug)
endif
endif

SOURCES = \
	crn_arealist.cpp \
	crn_assert.cpp \
	crn_checksum.cpp \
	crn_colorized_console.cpp \
	crn_command_line_params.cpp \
	crn_comp.cpp \
	crn_console.cpp \
	crn_core.cpp \
	crn_data_stream.cpp \
	crn_dds_comp.cpp \
	crn_decomp.cpp \
	crn_dxt.cpp \
	crn_dxt1.cpp \
	crn_dxt5a.cpp \
	crn_dxt_endpoint_refiner.cpp \
	crn_dxt_fast.cpp \
	crn_dxt_hc.cpp \
	crn_dxt_hc_common.cpp \
	crn_dxt_image.cpp \
	crn_dynamic_string.cpp \
	crn_etc.cpp \
	crn_file_utils.cpp \
	crn_find_files.cpp \
	crn_hash.cpp \
	crn_hash_map.cpp \
	crn_huffman_codes.cpp \
	crn_image_utils.cpp \
	crn_jpgd.cpp \
	crn_jpge.cpp \
	crn_ktx_texture.cpp \
	crn_lzma_codec.cpp \
	crn_math.cpp \
	crn_mem.cpp \
	crn_miniz.cpp \
	crn_mipmapped_texture.cpp \
	crn_pixel_format.cpp \
	crn_platform.cpp \
	crn_prefix_coding.cpp \
	crn_qdxt1.cpp \
	crn_qdxt5.cpp \
	crn_rand.cpp \
	crn_resample_filters.cpp \
	crn_resampler.cpp \
	crn_rg_etc1.cpp \
	crn_ryg_dxt.cpp \
	crn_sparse_bit_array.cpp \
	crn_stb_image.cpp \
	crn_strutils.cpp \
	crn_symbol_codec.cpp \
	crn_texture_comp.cpp \
	crn_texture_conversion.cpp \
	crn_texture_file_types.cpp \
	crn_threaded_resampler.cpp \
	crn_threading.cpp \
	crn_timer.cpp \
	crn_utils.cpp \
	crn_value.cpp \
	crn_vector.cpp \
	crnlib.cpp \
	lzma_7zBuf.cpp \
	lzma_7zBuf2.cpp \
	lzma_7zCrc.cpp \
	lzma_7zFile.cpp \
	lzma_7zStream.cpp \
	lzma_Alloc.cpp \
	lzma_Bcj2.cpp \
	lzma_Bra.cpp \
	lzma_Bra86.cpp \
	lzma_BraIA64.cpp \
	lzma_LzFind.cpp \
	lzma_LzmaDec.cpp \
	lzma_LzmaEnc.cpp \
	lzma_LzmaLib.cpp

HEADERS = \
	crn_arealist.h \
	crn_assert.h \
	crn_atomics.h \
	crn_buffer_stream.h \
	crn_cfile_stream.h \
	crn_checksum.h \
	crn_clusterizer.h \
	crn_color.h \
	crn_colorized_console.h \
	crn_command_line_params.h \
	crn_comp.h \
	crn_console.h \
	crn_core.h \
	crn_data_stream.h \
	crn_data_stream_serializer.h \
	crn_dds_comp.h \
	crn_dxt.h \
	crn_dxt1.h \
	crn_dxt5a.h \
	crn_dxt_endpoint_refiner.h \
	crn_dxt_fast.h \
	crn_dxt_hc.h \
	crn_dxt_hc_common.h \
	crn_dxt_image.h \
	crn_dynamic_stream.h \
	crn_dynamic_string.h \
	crn_etc.h \
	crn_file_utils.h \
	crn_find_files.h \
	crn_freeimage_image_utils.h \
	crn_hash.h \
	crn_hash_map.h \
	crn_helpers.h \
	crn_huffman_codes.h \
	crn_image.h \
	crn_image_utils.h \
	crn_intersect.h \
	crn_jpgd.h \
	crn_jpge.h \
	crn_ktx_texture.h \
	crn_lzma_codec.h \
	crn_math.h \
	crn_matrix.h \
	crn_mem.h \
	crn_miniz.h \
	crn_mipmapped_texture.h \
	crn_packed_uint.h \
	crn_pixel_format.h \
	crn_platform.h \
	crn_prefix_coding.h \
	crn_qdxt1.h \
	crn_qdxt5.h \
	crn_radix_sort.h \
	crn_rand.h \
	crn_ray.h \
	crn_rect.h \
	crn_resample_filters.h \
	crn_resampler.h \
	crn_rg_etc1.h \
	crn_sparse_array.h \
	crn_sparse_bit_array.h \
	crn_strutils.h \
	crn_symbol_codec.h \
	crn_texture_comp.h \
	crn_texture_conversion.h \
	crn_texture_file_types.h \
	crn_threaded_clusterizer.h \
	crn_threaded_resampler.h \
	crn_threading.h \
	crn_timer.h \
	crn_traits.h \
	crn_tree_clusterizer.h \
	crn_types.h \
	crn_utils.h \
	crn_value.h \
	crn_vec.h \
	crn_vec_interval.h \
	crn_vector.h \
	crn_vector2d.h \
	lzham_timer.h \
	lzma_7zBuf.h \
	lzma_7zCrc.h \
	lzma_7zFile.h \
	lzma_7zVersion.h \
	lzma_Alloc.h \
	lzma_Bcj2.h \
	lzma_Bra.h \
	lzma_CpuArch.h \
	lzma_LzFind.h \
	lzma_LzHash.h \
	lzma_LzmaDec.h \
	lzma_LzmaEnc.h \
	lzma_LzmaLib.h \
	lzma_MyVersion.h \
	lzma_Types.h

PUBLIC_HEADERS = \
	../inc/crn_decomp.h \
	../inc/crn_defs.h \
	../inc/crnlib.h \
	../inc/dds_defs.h

BINARY_SOURCES = \
	../crunch/crunch.cpp \
	../crunch/corpus_gen.cpp \
	../crunch/corpus_test.cpp

BINARY_HEADERS = \
	../crunch/corpus_gen.h \
	../crunch/corpus_test.h

BINARY = crunch

OBJECTS = $(SOURCES:.cpp=.o)

BINARY_OBJECTS = $(BINARY_SOURCES:.cpp=.o)

all: $(BINARY)

$(OBJECTS): %.o: %.cpp $(HEADERS) $(PUBLIC_HEADERS)
	@$(CXX) -c -o $@ $(abspath $<) $(CXXFLAGS)
	@echo "[Compile] $<"

$(BINARY_OBJECTS): %.o: %.cpp $(BINARY_HEADERS) $(PUBLIC_HEADERS) $(HEADERS)
	@$(CXX) -c -o $@ $(abspath $<) -I../inc -I../crnlib $(CXXFLAGS)
	@echo "[Compile] $<"

$(BINARY): $(OBJECTS) $(BINARY_OBJECTS)
	@$(CXX) -o $@ $^ $(LDFLAGS)
	@echo "[Link] $@ ($(BUILD))"

clean:
	@rm -f $(OBJECTS)
	@rm -f $(BINARY_OBJECTS)
	@rm -f $(BINARY)
	@echo "[Clean] Removing $(BINARY) build outputs"
