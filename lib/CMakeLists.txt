# Copyright 2011,2012,2016 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

########################################################################
# Setup library
########################################################################
include(GrPlatform) #define LIB_SUFFIX

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})
list(APPEND dvbt2rx_sources
    p1_demod_impl.cc
    gi_est_control_cc_impl.cc
    gi_est_decider_b_impl.cc )

set(dvbt2rx_sources "${dvbt2rx_sources}" PARENT_SCOPE )
if(NOT dvbt2rx_sources)
	MESSAGE(STATUS "No C++ sources... skipping lib/")
	return()
endif(NOT dvbt2rx_sources)

add_library(gnuradio-dvbt2rx SHARED ${dvbt2rx_sources})
target_link_libraries(gnuradio-dvbt2rx ${Boost_LIBRARIES} ${GNURADIO_ALL_LIBRARIES} ${FFTW3F_LIBRARIES})
set_target_properties(gnuradio-dvbt2rx PROPERTIES DEFINE_SYMBOL "gnuradio_dvbt2rx_EXPORTS")

if(APPLE)
    set_target_properties(gnuradio-dvbt2rx PROPERTIES
        INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
    )
endif(APPLE)

########################################################################
# Install built library files
########################################################################
include(GrMiscUtils)
GR_LIBRARY_FOO(gnuradio-dvbt2rx RUNTIME_COMPONENT "dvbt2rx_runtime" DEVEL_COMPONENT "dvbt2rx_devel")

########################################################################
# Build and register unit test
########################################################################
include(GrTest)

include_directories(${CPPUNIT_INCLUDE_DIRS})

list(APPEND test_dvbt2rx_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/test_dvbt2rx.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_dvbt2rx.cc
)

add_executable(test-dvbt2rx ${test_dvbt2rx_sources})

target_link_libraries(
  test-dvbt2rx
  ${GNURADIO_RUNTIME_LIBRARIES}
  ${Boost_LIBRARIES}
  ${CPPUNIT_LIBRARIES}
  gnuradio-dvbt2rx
)

GR_ADD_TEST(test_dvbt2rx test-dvbt2rx)

########################################################################
# Print summary
########################################################################
message(STATUS "Using install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Building for version: ${VERSION} / ${LIBVER}")

