# Check the OS type.

# CMake does not distinguish Linux from other Unices.
STRING(REGEX MATCH "Linux" PROJECT_OS_LINUX ${CMAKE_SYSTEM_NAME})
# Nor *BSD
STRING(REGEX MATCH "BSD" PROJECT_OS_BSD ${CMAKE_SYSTEM_NAME})
# Or Solaris. I'm seeing a trend, here
STRING(REGEX MATCH "SunOS" PROJECT_OS_SOLARIS ${CMAKE_SYSTEM_NAME})

IF(WIN32)
    SET(PROJECT_OS_WIN TRUE BOOL INTERNAL)
ENDIF(WIN32)

IF(APPLE)
    STRING(REGEX MATCH "Darwin" PROJECT_OS_OSX ${CMAKE_SYSTEM_NAME})
ENDIF(APPLE)

IF(PROJECT_OS_LINUX)
    MESSAGE(STATUS "Operating system is Linux")
ELSEIF(PROJECT_OS_BSD)
    MESSAGE(STATUS "Operating system is BSD")
ELSEIF(PROJECT_OS_WIN)
    MESSAGE(STATUS "Operating system is Windows")
ELSEIF(PROJECT_OS_OSX)
    MESSAGE(STATUS "Operating system is Apple MacOS X")
ELSEIF(PROJECT_OS_SOLARIS)
    MESSAGE(STATUS "Operating system is Solaris")
ELSE(PROJECT_OS_LINUX)
    MESSAGE(STATUS "Operating system is generic Unix")
ENDIF(PROJECT_OS_LINUX)

message(STATUS "CMAKE_SYSTEM_PROCESSOR: ${CMAKE_SYSTEM_PROCESSOR}")

# 32 or 64 bit
IF(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
    SET(PROJECT_PROC_64BIT TRUE BOOL INTERNAL)
    MESSAGE(STATUS "Linux x86_64 Detected")
ENDIF()
