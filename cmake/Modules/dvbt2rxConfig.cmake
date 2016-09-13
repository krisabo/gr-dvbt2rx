INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_DVBT2RX dvbt2rx)

FIND_PATH(
    DVBT2RX_INCLUDE_DIRS
    NAMES dvbt2rx/api.h
    HINTS $ENV{DVBT2RX_DIR}/include
        ${PC_DVBT2RX_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    DVBT2RX_LIBRARIES
    NAMES gnuradio-dvbt2rx
    HINTS $ENV{DVBT2RX_DIR}/lib
        ${PC_DVBT2RX_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(DVBT2RX DEFAULT_MSG DVBT2RX_LIBRARIES DVBT2RX_INCLUDE_DIRS)
MARK_AS_ADVANCED(DVBT2RX_LIBRARIES DVBT2RX_INCLUDE_DIRS)

