add_subdirectory(utils)
add_subdirectory(calib)
add_subdirectory(geometry)
add_subdirectory(alloc)
add_subdirectory(peaks)
add_subdirectory(digitizer)
add_subdirectory(detector)
add_subdirectory(shmem)

add_library(psalg SHARED
    calib/src/ArrayIO.cc
###   utils/src/Logger.cc
)

target_include_directories(psalg PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

message(STATUS "psalg/CMakeLists: CMAKE_CURRENT_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "psalg/CMakeLists: CMAKE_SOURCE_DIR=${CMAKE_SOURCE_DIR}")
message(STATUS "psalg/CMakeLists: CURL_LIBRARY=${CURL_LIBRARY}")
message(STATUS "psalg/CMakeLists: CONDA_PREFIX=$ENV{CONDA_PREFIX}")

# this is necessary only on RHEL6 where we need to pick up clock_gettime
if(APPLE)
    target_link_libraries(psalg
        utils
        calib
        geometry
        peaks
        digitizer
        detector
    )
else()
    target_link_libraries(psalg
        utils
        calib
        geometry
        peaks
        digitizer
        detector
        rt
    )
endif()

# datareader - test of data reader from non-xtc files
add_executable(datareader
    app/datareader.cc
)
target_link_libraries(datareader
    detector
    psalg
    xtcdata::xtc
)

add_executable(hsd_valid tests/hsd_valid.cc)
target_include_directories(hsd_valid PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
)

target_link_libraries(hsd_valid
    psalg
    xtcdata::xtc
)

# Needed for valgrind
include(CTest) # ctest -T memcheck

# Test 1: AllocArray
add_executable(test_array
    tests/test_array.cc
)
target_link_libraries(test_array
    psalg
    xtcdata::xtc
)
add_test(NAME test_array COMMAND ${CMAKE_BINARY_DIR}/psalg/test_array
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# Test 2: Peak finder
add_executable(test_peakFinder
    tests/peakHeap.cc
)
target_link_libraries(test_peakFinder
    psalg
    xtcdata::xtc
)
add_test(NAME test_peakFinder COMMAND ${CMAKE_BINARY_DIR}/psalg/test_peakFinder
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# Test 3: Hsd
add_executable(test_hsd
    tests/test_hsd.cc
)
target_link_libraries(test_hsd
    psalg
    xtcdata::xtc
)
add_test(NAME test_hsd COMMAND ${CMAKE_BINARY_DIR}/psalg/test_hsd
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# Test Logger
add_executable(test_logger
    tests/test_logger.cc
)
target_link_libraries(test_logger
    psalg
    xtcdata::xtc
)

# Test CalibPars
add_executable(test_CalibPars
    tests/test_CalibPars.cc
)
target_link_libraries(test_CalibPars
    psalg
    xtcdata::xtc
)

# Test SegGeometry
add_executable(test_SegGeometry
    tests/test_SegGeometry.cc
)
target_link_libraries(test_SegGeometry
    psalg
    xtcdata::xtc
)

# Test GeometryAccess
add_executable(test_GeometryAccess
    tests/test_GeometryAccess.cc
)
target_compile_options(test_GeometryAccess PRIVATE ${OpenMP_CXX_FLAGS})
target_link_libraries(test_GeometryAccess PRIVATE ${OpenMP_CXX_FLAGS}
    psalg
    xtcdata::xtc
)

# Test AreaDetector
add_executable(test_AreaDetector
    tests/test_AreaDetector.cc
)
target_link_libraries(test_AreaDetector
    psalg
    xtcdata::xtc
)

# Test curl
add_executable(test_MDBWebUtils
    tests/test_MDBWebUtils.cc
)
target_link_libraries(test_MDBWebUtils
    psalg
    xtcdata::xtc
    CURL::libcurl
)

# Test access to xtc data
add_executable(test_xtc_data
    tests/test_xtc_data.cc
)
target_link_libraries(test_xtc_data
    psalg
    xtcdata::xtc
)
add_test(NAME test_xtc_data COMMAND ${CMAKE_BINARY_DIR}/psalg/test_xtc_data
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS psalg
                test_array
                test_peakFinder
                test_hsd
                test_CalibPars
                test_SegGeometry
                test_GeometryAccess
                test_AreaDetector
                test_MDBWebUtils
                test_logger
                test_xtc_data
                datareader
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
