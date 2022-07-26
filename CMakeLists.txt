# Kokkos requires 3.16
CMAKE_MINIMUM_REQUIRED(VERSION 3.16)

# Project Tacho
PROJECT(Tacho VERSION 1.0.0 LANGUAGES C CXX)

# Built-in modules
INCLUDE(GNUInstallDirs)
INCLUDE(CMakePackageConfigHelpers)

# CMAKE setup
SET(CMAKE_VERBOSE_MAKEFILE ON) # see all output
SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR})

# Define BUILD_TESTING in cmake
INCLUDE(CTest)

# Installation
IF (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  SET(CMAKE_INSTALL_PREFIX "${PROJECT_BINARY_DIR}" CACHE PATH "Tacho install prefix" FORCE)
ENDIF()

IF (NOT CMAKE_INSTALL_LIBDIR)
  SET(CMAKE_INSTALL_LIBDIR lib)
ENDIF()
MESSAGE(STATUS "CMAKE_INSTALL_LIBDIR is set ${CMAKE_INSTALL_LIBDIR}")

SET(TACHO_INSTALL_BIN_PATH bin)
SET(TACHO_INSTALL_LIB_PATH ${CMAKE_INSTALL_LIBDIR})
SET(TACHO_INSTALL_INCLUDE_PATH include/tacho)
SET(TACHO_INSTALL_TEST_PATH unit-test)
SET(TACHO_INSTALL_EXAMPLE_PATH example)

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")
MESSAGE(STATUS "TACHO uses CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}")

#
# Options :
#
OPTION(TACHO_ENABLE_VERBOSE "Flag to enable Tacho verbose flag" OFF)
OPTION(TACHO_ENABLE_DEBUG "Flag to enable Tacho debug flag" OFF)
OPTION(TACHO_ENABLE_TEST "Flag to enable unit tests" OFF)
OPTION(TACHO_ENABLE_EXAMPLE "Flag to enable unit examples" ON)
OPTION(TACHO_ENABLE_PYTHON "Flag to enable python binding" OFF)
OPTION(TACHO_ENABLE_FORTRAN "Flag to enable fortran binding" OFF)

OPTION(TACHO_ENABLE_ORDINAL_TYPE_INT "Flag to enable ordinal type int" ON)
OPTION(TACHO_ENABLE_SIZE_TYPE_INT "Flag to enable size type int" ON)
OPTION(TACHO_ENABLE_SIZE_TYPE_SIZE_T "Flag to enable size type int" OFF)

OPTION(KOKKOS_INSTALL_PATH "Path to Kokkos installation")
OPTION(METIS_INSTALL_PATH "Path to Metis installation")
OPTION(BLAS_INSTALL_PATH "Path to BLAS installation")
OPTION(LAPACK_INSTALL_PATH "Path to LAPACK installation")
OPTION(GTEST_INSTALL_PATH "Path to gtest installation")
OPTION(CUDA_INSTALL_PATH "Path to CUBLAS/CUSOLVER installation")


INCLUDE_DIRECTORIES(${TACHO_BINARY_DIR})

# TACHO link libraries for tests
SET(TACHO_LINK_LIBRARIES tacho)

#
# Kokkos
#
IF (KOKKOS_INSTALL_PATH)
  MESSAGE(STATUS "TACHO uses Kokkos installed at ${KOKKOS_INSTALL_PATH}")
  FIND_PACKAGE(Kokkos REQUIRED)
  SET(TACHO_INTERNAL_KOKKOS_TARGET "Kokkos::kokkos")
  SET(TACHO_ENABLE_KOKKOS ON CACHE BOOL "Enable Kokkos")
  LIST(APPEND TACHO_LINK_LIBRARIES ${TACHO_INTERNAL_KOKKOS_TARGET})
ELSE()
  MESSAGE(FATAL_ERROR "-- Kokkos is required library but KOKKOS_INSTALL_PATH is not provided")
ENDIF()

#
# Metis
#
IF (METIS_INSTALL_PATH)
  FIND_PACKAGE (Metis)
  IF (METIS_FOUND)
    SET(TACHO_ENABLE_METIS ON CACHE BOOL "Enable Metis")
    SET(TACHO_INTERNAL_METIS_TARGET "metis")
    # MESSAGE("-- Metis is found at ${METIS_INSTALL_PATH}")
    LIST(APPEND TACHO_LINK_LIBRARIES metis)
  ELSE()
    MESSAGE(FATAL_ERROR "-- Metis is not found at ${METIS_INSTALL_PATH}")
  ENDIF()
ENDIF()

#
# Gtest
#
IF (TACHO_ENABLE_TEST)
  IF (GTEST_INSTALL_PATH)
    # Interface installed gtest
    MESSAGE("-- TACHO uses gtest installed at ${GTEST_INSTALL_PATH}")
    FIND_PACKAGE(Gtest REQUIRED)
    SET(TACHO_INTERNAL_GTEST_TARGET "GTest::gtest_main")
  ELSE()
    MESSAGE(FATAL_ERROR "-- TEST is enabled but GTEST_INSTALL_PATH is not provided")
  ENDIF()
  LIST(APPEND TACHO_LINK_LIBRARIES ${TACHO_INTERNAL_GTEST_TARGET})
ENDIF()

#
# MKL
#
IF (TACHO_ENABLE_MKL)
  IF (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
    SET(TACHO_ENABLE_BLAS ON CACHE BOOL "Enable BLAS")
    SET(TACHO_ENABLE_LAPACK ON CACHE BOOL "Enable LAPACK")
    SET(TACHO_ENABLE_MKL ON CACHE BOOL "Enable MKL")
    SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -mkl")
    SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -mkl")
    MESSAGE("-- MKL is enabled (use intel compiler supporting -mkl)")
  ELSE()
    MESSAGE(FATAL_ERROR "-- MKL is not enabled (use intel compiler supporting -mkl); MKL is not enabled")
  ENDIF()
ENDIF()

#
# OpenBLAS
#
IF (BLAS_INSTALL_PATH AND NOT TACHO_ENABLE_MKL)
  SET(OPENBLAS_INSTALL_PATH ${BLAS_INSTALL_PATH})
  FIND_PACKAGE (OpenBLAS)
  IF (OPENBLAS_FOUND)
    SET(TACHO_ENABLE_BLAS ON CACHE BOOL "Enable BLAS")
    SET(TACHO_ENABLE_LAPACK ON CACHE BOOL "Enable LAPACK")
    SET(TACHO_INTERNAL_OPENBLAS_TARGET "openblas")
    #MESSAGE("-- OpenBLAS is found at ${OPENBLAS_INSTALL_PATH}")
    LIST(APPEND TACHO_LINK_LIBRARIES openblas)
  ENDIF()
ENDIF()

#
# BLAS
#
IF (BLAS_INSTALL_PATH AND NOT TACHO_ENABLE_BLAS)
  FIND_PACKAGE (BLAS)
  IF (BLAS_FOUND)
    SET(TACHO_ENABLE_BLAS ON CACHE BOOL "Enable BLAS")
    SET(TACHO_INTERNAL_BLAS_TARGET "blas")
    #MESSAGE("-- BLAS is found at ${BLAS_INSTALL_PATH}")
    LIST(APPEND TACHO_LINK_LIBRARIES blas)
  ENDIF()
ENDIF()

#
# LAPACK
#
IF (LAPACK_INSTALL_PATH AND NOT TACHO_ENABLE_LAPACK)
  FIND_PACKAGE (LAPACK)
  IF (LAPACK_FOUND)
    SET(TACHO_ENABLE_LAPACK ON CACHE BOOL "Enable LAPACK")
    SET(TACHO_INTERNAL_LAPACK_TARGET "lapack")
    #MESSAGE("-- LAPACK is found at ${LAPACK_INSTALL_PATH}")
    LIST(APPEND TACHO_LINK_LIBRARIES lapack)
  ENDIF()
ENDIF()

#
# CUDA TPLs
#
IF (CUDA_INSTALL_PATH)
  # Interface installed CUDA TPLs
  MESSAGE("-- TACHO uses CUBLAS/CUSOLVER installed at ${CUDA_INSTALL_PATH}")
  FIND_PACKAGE(CUDA)
  IF (CUDA_FOUND)
    SET(TACHO_ENABLE_CUDA ON CACHE BOOL "Enable CUDA")
    SET(TACHO_INTERNAL_CUDA_TARGET "cusolver;cusparse;cublas")
    LIST(APPEND TACHO_LINK_LIBRARIES ${TACHO_INTERNAL_CUDA_TARGET})
  ELSE()
    MESSAGE(FATAL_ERROR "-- CUDA is not found from ${CUDA_INSTALL_PATH}")
  ENDIF()
ELSE()
  MESSAGE(WARNING "-- CUDA_INSTALL_PATH is not provided; CUDA TPLs are not enabled")
ENDIF()

#
# Linking libraries used in example and test
#
IF (NOT (TACHO_ENABLE_MKL))
  IF (NOT (TACHO_ENABLE_LAPACK AND TACHO_ENABLE_OPENBLAS))
    MESSAGE(WARNING "-- There no dense linear algebra packages are detected")
  ENDIF()
ENDIF()

MESSAGE("-- TACHO_LINK_LIBRARIES = ${TACHO_LINK_LIBRARIES}")

#
# Define configure-related preprocessor macros
#
CONFIGURE_FILE (
  ${CMAKE_SOURCE_DIR}/cmake/Tacho_Config.hpp.in
  ${CMAKE_BINARY_DIR}/Tacho_Config.hpp
  )
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/Tacho_Config.hpp
  DESTINATION "${CMAKE_INSTALL_PREFIX}/${TACHO_INSTALL_INCLUDE_PATH}")

#
# Process subdirectories
#
ADD_SUBDIRECTORY (core)
IF (TACHO_ENABLE_TEST AND TACHO_ENABLE_EXAMPLE)
  ADD_SUBDIRECTORY (unit-test)
ENDIF()
IF (TACHO_ENABLE_EXAMPLE OR TACHO_ENABLE_TEST)
  ADD_SUBDIRECTORY (example)
ENDIF()

