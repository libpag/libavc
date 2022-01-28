cmake_minimum_required(VERSION 3.5)

PROJECT(libavc)
enable_language(ASM)
#set(CMAKE_VERBOSE_MAKEFILE ON)

set(AVC_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(AVC_CONFIG_DIR "${CMAKE_CURRENT_BINARY_DIR}")

if ("${AVC_ROOT}" STREQUAL "${AVC_CONFIG_DIR}")
    message(
            FATAL_ERROR
            "Building from within the libavc source tree is not supported.\n"
            "Hint: Run these commands\n"
            "$ rm -rf CMakeCache.txt CMakeFiles\n"
            "$ mkdir -p ./build\n"
            "$ cd ./build\n"
            "And re-run CMake from the build directory.")
endif ()

set(CMAKE_STATIC_LIBRARY_PREFIX "")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w")
SET(CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS} -x assembler-with-cpp")

if (SANITIZE)
    string(TOLOWER ${SANITIZE} SANITIZE)
    set(CMAKE_SANITIZER_C_FLAGS "-fno-omit-frame-pointer -fsanitize=${SANITIZE}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_SANITIZER_C_FLAGS}")
endif ()

if (NOT ARCH)
    if (CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
        set(ARCH arm64)
    elseif (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
        set(ARCH x86)
    elseif (ANDROID OR IOS)
        set(ARCH arm)
    else ()
        set(ARCH x86)
    endif ()
endif ()

if (NOT WIN32)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    list(APPEND SYSTEM_LIBS Threads::Threads)
endif ()

if (ANDROID)
    find_library(LOG_LIB log)
    list(APPEND SYSTEM_LIBS ${LOG_LIB})
endif ()

list(APPEND LIBAVCDEC_SRCS
        "${AVC_ROOT}/common/ih264_buf_mgr.c"
        "${AVC_ROOT}/common/ih264_disp_mgr.c"
        "${AVC_ROOT}/common/ih264_inter_pred_filters.c"
        "${AVC_ROOT}/common/ih264_luma_intra_pred_filters.c"
        "${AVC_ROOT}/common/ih264_chroma_intra_pred_filters.c"
        "${AVC_ROOT}/common/ih264_padding.c"
        "${AVC_ROOT}/common/ih264_mem_fns.c"
        "${AVC_ROOT}/common/ih264_deblk_edge_filters.c"
        "${AVC_ROOT}/common/ih264_iquant_itrans_recon.c"
        "${AVC_ROOT}/common/ih264_ihadamard_scaling.c"
        "${AVC_ROOT}/common/ih264_weighted_pred.c"
        "${AVC_ROOT}/common/ithread.c"
        "${AVC_ROOT}/decoder/ih264d_cabac.c"
        "${AVC_ROOT}/decoder/ih264d_parse_mb_header.c"
        "${AVC_ROOT}/decoder/ih264d_parse_cabac.c"
        "${AVC_ROOT}/decoder/ih264d_process_intra_mb.c"
        "${AVC_ROOT}/decoder/ih264d_parse_bslice.c"
        "${AVC_ROOT}/decoder/ih264d_parse_pslice.c"
        "${AVC_ROOT}/decoder/ih264d_parse_islice.c"
        "${AVC_ROOT}/decoder/ih264d_cabac_init_tables.c"
        "${AVC_ROOT}/decoder/ih264d_bitstrm.c"
        "${AVC_ROOT}/decoder/ih264d_compute_bs.c"
        "${AVC_ROOT}/decoder/ih264d_deblocking.c"
        "${AVC_ROOT}/decoder/ih264d_parse_headers.c"
        "${AVC_ROOT}/decoder/ih264d_mb_utils.c"
        "${AVC_ROOT}/decoder/ih264d_mvpred.c"
        "${AVC_ROOT}/decoder/ih264d_utils.c"
        "${AVC_ROOT}/decoder/ih264d_process_bslice.c"
        "${AVC_ROOT}/decoder/ih264d_process_pslice.c"
        "${AVC_ROOT}/decoder/ih264d_parse_slice.c"
        "${AVC_ROOT}/decoder/ih264d_quant_scaling.c"
        "${AVC_ROOT}/decoder/ih264d_parse_cavlc.c"
        "${AVC_ROOT}/decoder/ih264d_dpb_mgr.c"
        "${AVC_ROOT}/decoder/ih264d_nal.c"
        "${AVC_ROOT}/decoder/ih264d_sei.c"
        "${AVC_ROOT}/decoder/ih264d_tables.c"
        "${AVC_ROOT}/decoder/ih264d_vui.c"
        "${AVC_ROOT}/decoder/ih264d_format_conv.c"
        "${AVC_ROOT}/decoder/ih264d_thread_parse_decode.c"
        "${AVC_ROOT}/decoder/ih264d_api.c"
        "${AVC_ROOT}/decoder/ih264d_thread_compute_bs.c"
        "${AVC_ROOT}/decoder/ih264d_function_selector_generic.c")
if (${ARCH} STREQUAL "arm64")
    set(LIBAVCDEC_ARCH_C_FLAGS
            "-DARMV8 -DARM -DDEFAULT_ARCH=D_ARCH_ARMV8_GENERIC")
    set(LIBAVCDEC_ARCH_INCLUDES ${AVC_ROOT}/common/armv8 ${AVC_ROOT}/decoder/arm)
    list(APPEND LIBAVCDEC_ARCH_SRCS
            "${AVC_ROOT}/decoder/arm/ih264d_function_selector.c"
            "${AVC_ROOT}/decoder/arm/ih264d_function_selector_av8.c"
            "${AVC_ROOT}/common/armv8/ih264_intra_pred_chroma_av8.s"
            "${AVC_ROOT}/common/armv8/ih264_intra_pred_luma_16x16_av8.s"
            "${AVC_ROOT}/common/armv8/ih264_intra_pred_luma_4x4_av8.s"
            "${AVC_ROOT}/common/armv8/ih264_inter_pred_chroma_av8.s"
            "${AVC_ROOT}/common/armv8/ih264_inter_pred_filters_luma_horz_av8.s"
            "${AVC_ROOT}/common/armv8/ih264_inter_pred_filters_luma_vert_av8.s"
            "${AVC_ROOT}/common/armv8/ih264_inter_pred_luma_copy_av8.s"
            "${AVC_ROOT}/common/armv8/ih264_inter_pred_luma_horz_qpel_av8.s"
            "${AVC_ROOT}/common/armv8/ih264_inter_pred_luma_vert_qpel_av8.s"
            "${AVC_ROOT}/common/armv8/ih264_inter_pred_luma_horz_hpel_vert_hpel_av8.s"
            "${AVC_ROOT}/common/armv8/ih264_inter_pred_luma_horz_qpel_vert_qpel_av8.s"
            "${AVC_ROOT}/common/armv8/ih264_inter_pred_luma_horz_qpel_vert_hpel_av8.s"
            "${AVC_ROOT}/common/armv8/ih264_inter_pred_luma_horz_hpel_vert_qpel_av8.s"
            "${AVC_ROOT}/common/armv8/ih264_default_weighted_pred_av8.s"
            "${AVC_ROOT}/common/armv8/ih264_weighted_pred_av8.s"
            "${AVC_ROOT}/common/armv8/ih264_weighted_bi_pred_av8.s"
            "${AVC_ROOT}/common/armv8/ih264_deblk_chroma_av8.s"
            "${AVC_ROOT}/common/armv8/ih264_deblk_luma_av8.s"
            "${AVC_ROOT}/common/armv8/ih264_padding_neon_av8.s"
            "${AVC_ROOT}/common/armv8/ih264_iquant_itrans_recon_av8.s"
            "${AVC_ROOT}/common/armv8/ih264_iquant_itrans_recon_dc_av8.s"
            "${AVC_ROOT}/common/armv8/ih264_ihadamard_scaling_av8.s"
            "${AVC_ROOT}/common/armv8/ih264_intra_pred_luma_8x8_av8.s")
elseif (${ARCH} STREQUAL "arm")
    set(LIBAVCDEC_ARCH_C_FLAGS "-DARM -mfpu=neon -DDEFAULT_ARCH=D_ARCH_ARM_A9Q")
    set(LIBAVCDEC_ARCH_INCLUDES ${AVC_ROOT}/common/arm ${AVC_ROOT}/decoder/arm)
    list(APPEND LIBAVCDEC_ARCH_SRCS
            "${AVC_ROOT}/decoder/arm/ih264d_function_selector.c"
            "${AVC_ROOT}/common/arm/ih264_arm_memory_barrier.s"
            "${AVC_ROOT}/decoder/arm/ih264d_function_selector_a9q.c"
            "${AVC_ROOT}/common/arm/ih264_intra_pred_chroma_a9q.s"
            "${AVC_ROOT}/common/arm/ih264_intra_pred_luma_16x16_a9q.s"
            "${AVC_ROOT}/common/arm/ih264_intra_pred_luma_4x4_a9q.s"
            "${AVC_ROOT}/common/arm/ih264_intra_pred_luma_8x8_a9q.s"
            "${AVC_ROOT}/common/arm/ih264_inter_pred_chroma_a9q.s"
            "${AVC_ROOT}/common/arm/ih264_inter_pred_filters_luma_horz_a9q.s"
            "${AVC_ROOT}/common/arm/ih264_inter_pred_filters_luma_vert_a9q.s"
            "${AVC_ROOT}/common/arm/ih264_inter_pred_luma_copy_a9q.s"
            "${AVC_ROOT}/common/arm/ih264_inter_pred_luma_horz_qpel_a9q.s"
            "${AVC_ROOT}/common/arm/ih264_inter_pred_luma_vert_qpel_a9q.s"
            "${AVC_ROOT}/common/arm/ih264_inter_pred_luma_horz_hpel_vert_hpel_a9q.s"
            "${AVC_ROOT}/common/arm/ih264_inter_pred_luma_horz_qpel_vert_qpel_a9q.s"
            "${AVC_ROOT}/common/arm/ih264_inter_pred_luma_horz_qpel_vert_hpel_a9q.s"
            "${AVC_ROOT}/common/arm/ih264_inter_pred_luma_horz_hpel_vert_qpel_a9q.s"
            "${AVC_ROOT}/common/arm/ih264_default_weighted_pred_a9q.s"
            "${AVC_ROOT}/common/arm/ih264_weighted_pred_a9q.s"
            "${AVC_ROOT}/common/arm/ih264_weighted_bi_pred_a9q.s"
            "${AVC_ROOT}/common/arm/ih264_deblk_chroma_a9.s"
            "${AVC_ROOT}/common/arm/ih264_deblk_luma_a9.s"
            "${AVC_ROOT}/common/arm/ih264_padding_neon.s"
            "${AVC_ROOT}/common/arm/ih264_iquant_itrans_recon_a9.s"
            "${AVC_ROOT}/common/arm/ih264_iquant_itrans_recon_dc_a9.s"
            "${AVC_ROOT}/common/arm/ih264_ihadamard_scaling_a9.s")
else ()
    set(LIBAVCDEC_ARCH_C_FLAGS
            "-DX86 -msse4.2 -DDEFAULT_ARCH=D_ARCH_X86_SSE42")
    set(LIBAVCDEC_ARCH_INCLUDES ${AVC_ROOT}/common/x86 ${AVC_ROOT}/decoder/x86)
    list(APPEND LIBAVCDEC_ARCH_SRCS
            "${AVC_ROOT}/decoder/x86/ih264d_function_selector.c"
            "${AVC_ROOT}/decoder/x86/ih264d_function_selector_sse42.c"
            "${AVC_ROOT}/decoder/x86/ih264d_function_selector_ssse3.c"
            "${AVC_ROOT}/common/x86/ih264_inter_pred_filters_ssse3.c"
            "${AVC_ROOT}/common/x86/ih264_deblk_luma_ssse3.c"
            "${AVC_ROOT}/common/x86/ih264_deblk_chroma_ssse3.c"
            "${AVC_ROOT}/common/x86/ih264_padding_ssse3.c"
            "${AVC_ROOT}/common/x86/ih264_mem_fns_ssse3.c"
            "${AVC_ROOT}/common/x86/ih264_iquant_itrans_recon_dc_ssse3.c"
            "${AVC_ROOT}/common/x86/ih264_iquant_itrans_recon_ssse3.c"
            "${AVC_ROOT}/common/x86/ih264_luma_intra_pred_filters_ssse3.c"
            "${AVC_ROOT}/common/x86/ih264_chroma_intra_pred_filters_ssse3.c"
            "${AVC_ROOT}/common/x86/ih264_iquant_itrans_recon_sse42.c"
            "${AVC_ROOT}/common/x86/ih264_weighted_pred_sse42.c"
            "${AVC_ROOT}/common/x86/ih264_ihadamard_scaling_sse42.c")
endif ()

set(LIBAVCDEC_INCLUDES ${AVC_ROOT}/common ${AVC_ROOT}/decoder)
set(LIBAVCDEC_C_FLAGS "${LIBAVCDEC_ARCH_C_FLAGS}")

include_directories(${LIBAVCDEC_INCLUDES} ${LIBAVCDEC_ARCH_INCLUDES})

include_directories(${WEBP_DEP_INCLUDE_DIRS})
if (IOS)
    string(REPLACE "-O2" "" CMAKE_C_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE})
    string(REPLACE "-O3" "" CMAKE_C_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE})
    string(REPLACE "-Os" "" CMAKE_C_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE})
    string(REPLACE "-O3" "" CMAKE_ASM_FLAGS_RELEASE ${CMAKE_ASM_FLAGS_RELEASE})
    # compiling ih264d_inter_pred with '-Os' flag causes crash at runtime.
    add_library(libavcdec-O2 OBJECT "${AVC_ROOT}/decoder/ih264d_inter_pred.c")
    add_library(libavcdec-Os OBJECT ${LIBAVCDEC_SRCS} ${LIBAVCDEC_ARCH_SRCS})
    set_target_properties(libavcdec-O2 PROPERTIES COMPILE_FLAGS "${LIBAVCDEC_C_FLAGS} -O2")
    set_target_properties(libavcdec-Os PROPERTIES COMPILE_FLAGS "${LIBAVCDEC_C_FLAGS} -Os")
    add_library(libavcdec $<TARGET_OBJECTS:libavcdec-O2> $<TARGET_OBJECTS:libavcdec-Os>)
else ()
    add_library(libavcdec ${LIBAVCDEC_SRCS} ${LIBAVCDEC_ARCH_SRCS} "${AVC_ROOT}/decoder/ih264d_inter_pred.c")
    set_target_properties(libavcdec PROPERTIES COMPILE_FLAGS "${LIBAVCDEC_C_FLAGS}")
endif ()

list(APPEND AVCDEC_SRCS "${AVC_ROOT}/test/decoder/main.c")

add_executable(avcdec ${AVCDEC_SRCS})
set_target_properties(avcdec
        PROPERTIES COMPILE_FLAGS "-DMD5_DISABLE -DPROFILE_ENABLE")
target_link_libraries(avcdec libavcdec ${SYSTEM_LIBS})

list(APPEND LIBAVCENC_SRCS
        "${AVC_ROOT}/common/ih264_resi_trans_quant.c"
        "${AVC_ROOT}/common/ih264_iquant_itrans_recon.c"
        "${AVC_ROOT}/common/ih264_ihadamard_scaling.c"
        "${AVC_ROOT}/common/ih264_inter_pred_filters.c"
        "${AVC_ROOT}/common/ih264_luma_intra_pred_filters.c"
        "${AVC_ROOT}/common/ih264_chroma_intra_pred_filters.c"
        "${AVC_ROOT}/common/ih264_padding.c"
        "${AVC_ROOT}/common/ih264_mem_fns.c"
        "${AVC_ROOT}/common/ih264_deblk_edge_filters.c"
        "${AVC_ROOT}/common/ih264_deblk_tables.c"
        "${AVC_ROOT}/common/ih264_cavlc_tables.c"
        "${AVC_ROOT}/common/ih264_cabac_tables.c"
        "${AVC_ROOT}/common/ih264_common_tables.c"
        "${AVC_ROOT}/common/ih264_trans_data.c"
        "${AVC_ROOT}/common/ih264_buf_mgr.c"
        "${AVC_ROOT}/common/ih264_dpb_mgr.c"
        "${AVC_ROOT}/common/ih264_list.c"
        "${AVC_ROOT}/common/ithread.c"
        "${AVC_ROOT}/encoder/ih264e_globals.c"
        "${AVC_ROOT}/encoder/ih264e_intra_modes_eval.c"
        "${AVC_ROOT}/encoder/ih264e_half_pel.c"
        "${AVC_ROOT}/encoder/ih264e_mc.c"
        "${AVC_ROOT}/encoder/ih264e_me.c"
        "${AVC_ROOT}/encoder/ih264e_rc_mem_interface.c"
        "${AVC_ROOT}/encoder/ih264e_time_stamp.c"
        "${AVC_ROOT}/encoder/ih264e_modify_frm_rate.c"
        "${AVC_ROOT}/encoder/ih264e_rate_control.c"
        "${AVC_ROOT}/encoder/ih264e_core_coding.c"
        "${AVC_ROOT}/encoder/ih264e_deblk.c"
        "${AVC_ROOT}/encoder/ih264e_api.c"
        "${AVC_ROOT}/encoder/ih264e_process.c"
        "${AVC_ROOT}/encoder/ih264e_encode.c"
        "${AVC_ROOT}/encoder/ih264e_utils.c"
        "${AVC_ROOT}/encoder/ih264e_version.c"
        "${AVC_ROOT}/encoder/ih264e_bitstream.c"
        "${AVC_ROOT}/encoder/ih264e_cavlc.c"
        "${AVC_ROOT}/encoder/ih264e_cabac_init.c"
        "${AVC_ROOT}/encoder/ih264e_cabac.c"
        "${AVC_ROOT}/encoder/ih264e_cabac_encode.c"
        "${AVC_ROOT}/encoder/ih264e_encode_header.c"
        "${AVC_ROOT}/encoder/ih264e_function_selector_generic.c"
        "${AVC_ROOT}/encoder/ih264e_fmt_conv.c"
        "${AVC_ROOT}/encoder/ih264e_sei.c"
        "${AVC_ROOT}/encoder/irc_rate_control_api.c"
        "${AVC_ROOT}/encoder/irc_bit_allocation.c"
        "${AVC_ROOT}/encoder/irc_cbr_buffer_control.c"
        "${AVC_ROOT}/encoder/irc_est_sad.c"
        "${AVC_ROOT}/encoder/irc_fixed_point_error_bits.c"
        "${AVC_ROOT}/encoder/irc_frame_info_collector.c"
        "${AVC_ROOT}/encoder/irc_mb_model_based.c"
        "${AVC_ROOT}/encoder/irc_picture_type.c"
        "${AVC_ROOT}/encoder/irc_rd_model.c"
        "${AVC_ROOT}/encoder/irc_vbr_storage_vbv.c"
        "${AVC_ROOT}/encoder/irc_vbr_str_prms.c"
        "${AVC_ROOT}/encoder/ime.c"
        "${AVC_ROOT}/encoder/ime_distortion_metrics.c")

if (${ARCH} STREQUAL "arm64")
    set(LIBAVCENC_ARCH_C_FLAGS "-DARMV8 -DARM")
    set(LIBAVCENC_ARCH_INCLUDES ${AVC_ROOT}/common/armv8 ${AVC_ROOT}/encoder/arm ${AVC_ROOT}/encoder/armv8)
    list(APPEND LIBAVCENC_ARCH_SRCS
            "${AVC_ROOT}/encoder/arm/ih264e_function_selector.c"
            "${AVC_ROOT}/encoder/arm/ih264e_function_selector_av8.c"
            "${AVC_ROOT}/common/armv8/ih264_resi_trans_quant_av8.s"
            "${AVC_ROOT}/common/armv8/ih264_iquant_itrans_recon_av8.s"
            "${AVC_ROOT}/common/armv8/ih264_iquant_itrans_recon_dc_av8.s"
            "${AVC_ROOT}/common/armv8/ih264_ihadamard_scaling_av8.s"
            "${AVC_ROOT}/common/armv8/ih264_intra_pred_chroma_av8.s"
            "${AVC_ROOT}/common/armv8/ih264_intra_pred_luma_16x16_av8.s"
            "${AVC_ROOT}/common/armv8/ih264_intra_pred_luma_4x4_av8.s"
            "${AVC_ROOT}/common/armv8/ih264_intra_pred_luma_8x8_av8.s"
            "${AVC_ROOT}/common/armv8/ih264_inter_pred_luma_copy_av8.s"
            "${AVC_ROOT}/common/armv8/ih264_inter_pred_chroma_av8.s"
            "${AVC_ROOT}/common/armv8/ih264_inter_pred_filters_luma_horz_av8.s"
            "${AVC_ROOT}/common/armv8/ih264_inter_pred_filters_luma_vert_av8.s"
            "${AVC_ROOT}/common/armv8/ih264_padding_neon_av8.s"
            "${AVC_ROOT}/common/armv8/ih264_mem_fns_neon_av8.s"
            "${AVC_ROOT}/common/armv8/ih264_deblk_luma_av8.s"
            "${AVC_ROOT}/common/armv8/ih264_deblk_chroma_av8.s"
            "${AVC_ROOT}/encoder/armv8/ih264e_evaluate_intra16x16_modes_av8.s"
            "${AVC_ROOT}/encoder/armv8/ih264e_evaluate_intra_chroma_modes_av8.s"
            "${AVC_ROOT}/encoder/armv8/ih264e_half_pel_av8.s"
            "${AVC_ROOT}/encoder/armv8/ime_distortion_metrics_av8.s")
elseif (${ARCH} STREQUAL "arm")
    set(LIBAVCENC_ARCH_C_FLAGS "-DARM -mfpu=neon -DDEFAULT_ARCH=D_ARCH_ARM_A9Q")
    set(LIBAVCENC_ARCH_INCLUDES ${AVC_ROOT}/common/arm ${AVC_ROOT}/encoder/arm)
    list(APPEND LIBAVCENC_ARCH_SRCS
            "${AVC_ROOT}/encoder/arm/ih264e_function_selector.c"
            "${AVC_ROOT}/common/arm/ih264_arm_memory_barrier.s"
            "${AVC_ROOT}/encoder/arm/ih264e_function_selector_a9q.c"
            "${AVC_ROOT}/common/arm/ih264_resi_trans_quant_a9.s"
            "${AVC_ROOT}/common/arm/ih264_iquant_itrans_recon_a9.s"
            "${AVC_ROOT}/common/arm/ih264_iquant_itrans_recon_dc_a9.s"
            "${AVC_ROOT}/common/arm/ih264_ihadamard_scaling_a9.s"
            "${AVC_ROOT}/common/arm/ih264_deblk_chroma_a9.s"
            "${AVC_ROOT}/common/arm/ih264_deblk_luma_a9.s"
            "${AVC_ROOT}/common/arm/ih264_intra_pred_chroma_a9q.s"
            "${AVC_ROOT}/common/arm/ih264_intra_pred_luma_16x16_a9q.s"
            "${AVC_ROOT}/common/arm/ih264_intra_pred_luma_4x4_a9q.s"
            "${AVC_ROOT}/common/arm/ih264_intra_pred_luma_8x8_a9q.s"
            "${AVC_ROOT}/common/arm/ih264_inter_pred_chroma_a9q.s"
            "${AVC_ROOT}/common/arm/ih264_inter_pred_filters_luma_horz_a9q.s"
            "${AVC_ROOT}/common/arm/ih264_inter_pred_filters_luma_vert_a9q.s"
            "${AVC_ROOT}/common/arm/ih264_inter_pred_luma_bilinear_a9q.s"
            "${AVC_ROOT}/common/arm/ih264_inter_pred_luma_copy_a9q.s"
            "${AVC_ROOT}/common/arm/ih264_padding_neon.s"
            "${AVC_ROOT}/common/arm/ih264_mem_fns_neon.s"
            "${AVC_ROOT}/encoder/arm/ih264e_evaluate_intra16x16_modes_a9q.s"
            "${AVC_ROOT}/encoder/arm/ih264e_evaluate_intra4x4_modes_a9q.s"
            "${AVC_ROOT}/encoder/arm/ih264e_evaluate_intra_chroma_modes_a9q.s"
            "${AVC_ROOT}/encoder/arm/ih264e_half_pel.s"
            "${AVC_ROOT}/encoder/arm/ih264e_fmt_conv.s"
            "${AVC_ROOT}/encoder/arm/ime_distortion_metrics_a9q.s")
else ()
    set(LIBAVCENC_ARCH_C_FLAGS "-DX86 -msse4.2")
    set(LIBAVCENC_ARCH_INCLUDES ${AVC_ROOT}/common/x86 ${AVC_ROOT}/encoder/x86)
    list(APPEND LIBAVCENC_ARCH_SRCS
            "${AVC_ROOT}/encoder/x86/ih264e_function_selector.c"
            "${AVC_ROOT}/encoder/x86/ih264e_function_selector_sse42.c"
            "${AVC_ROOT}/encoder/x86/ih264e_function_selector_ssse3.c"
            "${AVC_ROOT}/common/x86/ih264_iquant_itrans_recon_ssse3.c"
            "${AVC_ROOT}/common/x86/ih264_iquant_itrans_recon_dc_ssse3.c"
            "${AVC_ROOT}/common/x86/ih264_ihadamard_scaling_ssse3.c"
            "${AVC_ROOT}/common/x86/ih264_inter_pred_filters_ssse3.c"
            "${AVC_ROOT}/common/x86/ih264_mem_fns_ssse3.c"
            "${AVC_ROOT}/common/x86/ih264_padding_ssse3.c"
            "${AVC_ROOT}/common/x86/ih264_luma_intra_pred_filters_ssse3.c"
            "${AVC_ROOT}/common/x86/ih264_chroma_intra_pred_filters_ssse3.c"
            "${AVC_ROOT}/common/x86/ih264_deblk_chroma_ssse3.c"
            "${AVC_ROOT}/common/x86/ih264_deblk_luma_ssse3.c"
            "${AVC_ROOT}/common/x86/ih264_iquant_itrans_recon_sse42.c"
            "${AVC_ROOT}/common/x86/ih264_ihadamard_scaling_sse42.c"
            "${AVC_ROOT}/common/x86/ih264_resi_trans_quant_sse42.c"
            "${AVC_ROOT}/common/x86/ih264_weighted_pred_sse42.c"
            "${AVC_ROOT}/encoder/x86/ih264e_half_pel_ssse3.c"
            "${AVC_ROOT}/encoder/x86/ih264e_intra_modes_eval_ssse3.c"
            "${AVC_ROOT}/encoder/x86/ime_distortion_metrics_sse42.c")
endif ()


set(LIBAVCENC_INCLUDES ${AVC_ROOT}/common ${AVC_ROOT}/encoder)
set(LIBAVCENC_C_FLAGS "${LIBAVCENC_ARCH_C_FLAGS}")

include_directories(${LIBAVCENC_INCLUDES} ${LIBAVCENC_ARCH_INCLUDES})
add_library(libavcenc ${LIBAVCENC_SRCS} ${LIBAVCENC_ARCH_SRCS})
set_target_properties(libavcenc PROPERTIES COMPILE_FLAGS "${LIBAVCENC_C_FLAGS}")

list(APPEND AVCENC_SRCS
        "${AVC_ROOT}/test/encoder/main.c"
        "${AVC_ROOT}/test/encoder/psnr.c"
        "${AVC_ROOT}/test/encoder/input.c"
        "${AVC_ROOT}/test/encoder/output.c"
        "${AVC_ROOT}/test/encoder/recon.c")

add_executable(avcenc ${AVCENC_SRCS})
set_target_properties(avcenc
        PROPERTIES COMPILE_FLAGS
        "-DARM -DMD5_DISABLE -DPROFILE_ENABLE")
target_link_libraries(avcenc
        libavcenc
        ${SYSTEM_LIBS}
        m)
