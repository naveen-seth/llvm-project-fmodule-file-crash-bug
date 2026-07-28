set(TSAN_DD_LIT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(TSAN_DD_TEST_DEPS ${SANITIZER_COMMON_LIT_TEST_DEPS})
list(APPEND TSAN_DD_TEST_DEPS tsan_dd)

set(TSAN_DD_TESTSUITES)
set(TSAN_DD_TEST_ARCH ${TSAN_DD_SUPPORTED_ARCH})

foreach(arch ${TSAN_DD_TEST_ARCH})
  set(TSAN_DD_TEST_TARGET_ARCH ${arch})
  string(TOLOWER "-${arch}" TSAN_DD_TEST_CONFIG_SUFFIX)
  get_test_cc_for_arch(${arch} TSAN_DD_TEST_TARGET_CC TSAN_DD_TEST_TARGET_CFLAGS)

  string(TOUPPER ${arch} ARCH_UPPER_CASE)
  set(CONFIG_NAME ${ARCH_UPPER_CASE}Config)

  configure_lit_site_cfg(
    ${CMAKE_CURRENT_SOURCE_DIR}/lit.site.cfg.py.in
    ${CMAKE_CURRENT_BINARY_DIR}/${CONFIG_NAME}/lit.site.cfg.py
    MAIN_CONFIG
    ${CMAKE_CURRENT_SOURCE_DIR}/lit.cfg.py)

  list(APPEND TSAN_DD_TESTSUITES ${CMAKE_CURRENT_BINARY_DIR}/${CONFIG_NAME})
endforeach()

add_lit_testsuite(check-tsan_dd "Running ThreadSanitizer-DD tests"
  ${TSAN_DD_TESTSUITES}
  DEPENDS ${TSAN_DD_TEST_DEPS})
set_target_properties(check-tsan_dd PROPERTIES FOLDER "Compiler-RT Tests")
