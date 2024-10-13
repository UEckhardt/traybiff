cmake_minimum_required (VERSION 3.25)

project(traybiff
        VERSION 0.5.1
        LANGUAGES CXX)

include(FeatureSummary)


set(QT_MIN_VERSION "6.5.0")
set(CMAKE_CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED ON)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING
            "Default build type: Debug" FORCE)
endif()

find_package(Qt6Core ${QT_MIN_VERSION} REQUIRED)
find_package(Qt6Gui ${QT_MIN_VERSION} REQUIRED)
find_package(Qt6Network ${QT_MIN_VERSION} REQUIRED)
find_package(Qt6Widgets ${QT_MIN_VERSION} REQUIRED)
find_package(Qt6Keychain 0.14.0 REQUIRED)

if (NOT EXISTS /usr/lib64/qt6/plugins/iconengines/libqsvgicon.so)
  message(WARNING "libqsvgicon seems not be present")
endif()


feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

add_subdirectory(src)

