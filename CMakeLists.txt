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
set(SOURCES 
  src/nsvariant.cpp
  src/nsclassfactory.cpp
  src/nsdictionary.cpp
  src/nsarray.cpp
  src/nsset.cpp
  src/nsstring.cpp

  src/nskeyedunarchiver.cpp
)
add_library(${PROJECT_NAME} ${SOURCES})
target_link_libraries(
  ${PROJECT_NAME}
  PkgConfig::plist
)

# test
enable_testing()
add_executable(
  ${PROJECT_NAME}_test
  test/nskeyedunarchiver_test.cpp
)
target_link_libraries(
  ${PROJECT_NAME}_test
  ${PROJECT_NAME}
  gtest_main
)
include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME}_test)


