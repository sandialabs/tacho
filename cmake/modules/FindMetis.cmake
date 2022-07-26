# Check Metis installation
FIND_PATH(METIS_FOUND_INCLUDE_PATH metis.h
  HINTS ${METIS_INSTALL_PATH}/include ${METIS_INSTALL_PATH}/include/metis
  NO_DEFAULT_PATH
)
FIND_LIBRARY(METIS_FOUND_LIBRARY NAMES metis
  HINTS ${METIS_INSTALL_PATH}/lib ${METIS_INSTALL_PATH}/lib64
)

IF (METIS_FOUND_INCLUDE_PATH AND METIS_FOUND_LIBRARY)
  ADD_LIBRARY(metis INTERFACE)
  SET_TARGET_PROPERTIES(metis PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${METIS_FOUND_INCLUDE_PATH}
    INTERFACE_COMPILE_OPTIONS "-I${METIS_FOUND_INCLUDE_PATH}"
    INTERFACE_LINK_LIBRARIES ${METIS_FOUND_LIBRARY}
  )
  SET(METIS_FOUND ON)
  INSTALL(TARGETS metis
    EXPORT tacho-targets
    LIBRARY DESTINATION "${TACHO_INSTALL_LIB_PATH}"
    ARCHIVE DESTINATION "${TACHO_INSTALL_LIB_PATH}"
  )
  MESSAGE("-- Metis is found at ${METIS_FOUND_LIBRARY}")
ELSE()
  MESSAGE(FATAL_ERROR "-- Metis is not found at ${METIS_INSTALL_PATH}")
ENDIF()

