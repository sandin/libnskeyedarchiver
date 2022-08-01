cmake_minimum_required(VERSION 3.0.0)
project(nskeyedarchiver VERSION 0.1.0)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# libplist
if (NOT DEFINED ENV{IMB_PKG_CONFIG_PATH})
    message(FATAL_ERROR "not defined env var: IMB_PKG_CONFIG_PATH for imobiledevice") # "../perfcat_build_scripts/built/darwin-arm64/lib/pkgconfig"
endif()
set(ENV{PKG_CONFIG_PATH} $ENV{IMB_PKG_CONFIG_PATH})
find_package(PkgConfig)
pkg_check_modules(plist REQUIRED IMPORTED_TARGET libplist-2.0)
include_directories(${plist_INCLUDE_DIRS})

# library: nskeyedarchiver
include_directories("./include")
set(HEADERS
  include/nskeyedarchiver/base64.hpp
  include/nskeyedarchiver/common.hpp
  include/nskeyedarchiver/kaarray.hpp
  include/nskeyedarchiver/kamap.hpp
  include/nskeyedarchiver/kaobject.hpp
  include/nskeyedarchiver/kastring.hpp
  include/nskeyedarchiver/kavalue.hpp
  include/nskeyedarchiver/nstypes.hpp
  include/nskeyedarchiver/nsclass.hpp
  include/nskeyedarchiver/nsclassmanager.hpp
  include/nskeyedarchiver/nskeyedarchiver.hpp
  include/nskeyedarchiver/nskeyedunarchiver.hpp
  include/nskeyedarchiver/scope.hpp
)
set(SOURCES 
  src/base64.cpp
  src/nstypes.cpp
  src/nsclassmanager.cpp
  src/nskeyedarchiver.cpp
  src/nskeyedunarchiver.cpp
)
add_library(${PROJECT_NAME} ${HEADERS} ${SOURCES})
target_link_libraries(
  ${PROJECT_NAME}
  PkgConfig::plist
)

# tools: nskeyedarchiver_tools
add_executable(
  ${PROJECT_NAME}_tools
  tools/main.cpp
)
target_link_libraries(
  ${PROJECT_NAME}_tools
  ${PROJECT_NAME}
)

# test
enable_testing()
add_executable(
  ${PROJECT_NAME}_test
  test/kavalue_test.cpp
  test/kaobject_test.cpp
  test/kaarray_test.cpp
  test/kamap_test.cpp
  test/kastring_test.cpp
  test/base64_test.cpp
  test/nskeyedarchiver_test.cpp
  test/nskeyedunarchiver_test.cpp
)
target_link_libraries(
  ${PROJECT_NAME}_test
  ${PROJECT_NAME}
  gtest_main
)
include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME}_test)


# asan
#target_compile_options(${PROJECT_NAME} PUBLIC -fsanitize=address -fno-omit-frame-pointer)
#set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS -fsanitize=address)
#target_compile_options(${PROJECT_NAME}_test PUBLIC -fsanitize=address -fno-omit-frame-pointer)
#set_target_properties(${PROJECT_NAME}_test PROPERTIES LINK_FLAGS -fsanitize=address)