# Build for the ThreadSanitizer-DD (deadlock detector) runtime support library.

include_directories(..)

set(TSAN_DD_SOURCES
  tsan_dd_rtl.cpp
  tsan_dd_interceptors.cpp
  tsan_dd_interface.cpp)

set(TSAN_DD_HEADERS
  tsan_dd_rtl.h)

set(TSAN_DD_CFLAGS ${SANITIZER_COMMON_CFLAGS})
append_rtti_flag(OFF TSAN_DD_CFLAGS)

set(TSAN_DD_DYNAMIC_LINK_LIBS
  ${COMPILER_RT_UNWINDER_LINK_LIBS}
  ${SANITIZER_CXX_ABI_LIBRARIES}
  ${SANITIZER_COMMON_LINK_LIBS})

append_list_if(COMPILER_RT_HAS_LIBDL dl TSAN_DD_DYNAMIC_LINK_LIBS)
append_list_if(COMPILER_RT_HAS_LIBM m TSAN_DD_DYNAMIC_LINK_LIBS)
append_list_if(COMPILER_RT_HAS_LIBPTHREAD pthread TSAN_DD_DYNAMIC_LINK_LIBS)

add_compiler_rt_component(tsan_dd)

if(APPLE)
  add_weak_symbols("sanitizer_common" WEAK_SYMBOL_LINK_FLAGS)

  add_compiler_rt_runtime(clang_rt.tsan_dd
    SHARED
    OS ${TSAN_DD_SUPPORTED_OS}
    ARCHS ${TSAN_DD_SUPPORTED_ARCH}
    SOURCES ${TSAN_DD_SOURCES}
    ADDITIONAL_HEADERS ${TSAN_DD_HEADERS}
    OBJECT_LIBS RTInterception
                RTSanitizerCommon
                RTSanitizerCommonLibc
                RTSanitizerCommonCoverage
                RTSanitizerCommonSymbolizer
    CFLAGS ${TSAN_DD_CFLAGS}
    LINK_FLAGS ${SANITIZER_COMMON_LINK_FLAGS} ${WEAK_SYMBOL_LINK_FLAGS}
    LINK_LIBS ${SANITIZER_COMMON_LINK_LIBS}
    PARENT_TARGET tsan_dd)
else()
  foreach(arch ${TSAN_DD_SUPPORTED_ARCH})
    add_compiler_rt_runtime(clang_rt.tsan_dd
      STATIC
      ARCHS ${arch}
      SOURCES ${TSAN_DD_SOURCES}
              $<TARGET_OBJECTS:RTInterception.${arch}>
              $<TARGET_OBJECTS:RTSanitizerCommon.${arch}>
              $<TARGET_OBJECTS:RTSanitizerCommonLibc.${arch}>
              $<TARGET_OBJECTS:RTSanitizerCommonCoverage.${arch}>
              $<TARGET_OBJECTS:RTSanitizerCommonSymbolizer.${arch}>
              $<TARGET_OBJECTS:RTSanitizerCommonSymbolizerInternal.${arch}>
      ADDITIONAL_HEADERS ${TSAN_DD_HEADERS}
      CFLAGS ${TSAN_DD_CFLAGS}
      PARENT_TARGET tsan_dd)
    add_compiler_rt_runtime(clang_rt.tsan_dd
      SHARED
      ARCHS ${arch}
      SOURCES ${TSAN_DD_SOURCES}
              $<TARGET_OBJECTS:RTInterception.${arch}>
              $<TARGET_OBJECTS:RTSanitizerCommon.${arch}>
              $<TARGET_OBJECTS:RTSanitizerCommonLibc.${arch}>
              $<TARGET_OBJECTS:RTSanitizerCommonCoverage.${arch}>
              $<TARGET_OBJECTS:RTSanitizerCommonSymbolizer.${arch}>
              $<TARGET_OBJECTS:RTSanitizerCommonSymbolizerInternal.${arch}>
      ADDITIONAL_HEADERS ${TSAN_DD_HEADERS}
      CFLAGS ${TSAN_DD_CFLAGS}
      LINK_LIBS ${TSAN_DD_DYNAMIC_LINK_LIBS}
      LINK_FLAGS ${SANITIZER_COMMON_LINK_FLAGS}
      PARENT_TARGET tsan_dd)
  endforeach()
endif()
