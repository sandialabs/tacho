# Check OpenBLAS installation
FIND_PATH(OPENBLAS_FOUND_INCLUDE_PATH openblas_config.h
  HINTS ${OPENBLAS_INSTALL_PATH}/include ${OPENBLAS_INSTALL_PATH}/include/openblas
  NO_DEFAULT_PATH
)
FIND_LIBRARY(OPENBLAS_FOUND_LIBRARY NAMES openblas
  HINTS ${OPENBLAS_INSTALL_PATH}/lib ${OPENBLAS_INSTALL_PATH}/lib64
)

IF (OPENBLAS_FOUND_INCLUDE_PATH AND OPENBLAS_FOUND_LIBRARY)
  ADD_LIBRARY(openblas INTERFACE)
  SET_TARGET_PROPERTIES(openblas PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${OPENBLAS_FOUND_INCLUDE_PATH}
    INTERFACE_COMPILE_OPTIONS "-I${OPENBLAS_FOUND_INCLUDE_PATH}"
    INTERFACE_LINK_LIBRARIES ${OPENBLAS_FOUND_LIBRARY}
  )
  SET(OPENBLAS_FOUND ON)
  INSTALL(TARGETS openblas
    EXPORT tacho-targets
    LIBRARY DESTINATION "${TACHO_INSTALL_LIB_PATH}"
    ARCHIVE DESTINATION "${TACHO_INSTALL_LIB_PATH}"
  )
  MESSAGE("-- OpenBLAS is found at ${OPENBLAS_FOUND_LIBRARY}")
ENDIF()

