cmake_minimum_required (VERSION 3.25)

project(traybiff
        VERSION 0.6.0
        DESCRIPTION "TrayBiff - new mail notification utility for the system tray"
        LANGUAGES CXX)

include(FeatureSummary)


set(QT_MIN_VERSION "6.5.0")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
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

if (NOT TARGET Qt6::QSvgIconPlugin AND
    NOT EXISTS /usr/lib64/qt6/plugins/iconengines/libqsvgicon.so AND
    NOT EXISTS /usr/lib/x86_64-linux-gnu/qt6/plugins/iconengines/libqsvgicon.so)
  message(WARNING "libqsvgicon seems not be present")
endif()


feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

add_subdirectory(src)

# CPack configuration
set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_CONTACT "uli@uli-eckhardt.de")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_DESCRIPTION}")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING.txt")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

# Detect Debian-based or RPM-based system
set(IS_DEBIAN_SYSTEM FALSE)
set(IS_RPM_SYSTEM FALSE)
if(EXISTS "/etc/debian_version")
    set(IS_DEBIAN_SYSTEM TRUE)
elseif(EXISTS "/etc/redhat-release" OR EXISTS "/etc/SuSE-release" OR EXISTS "/etc/fedora-release")
    set(IS_RPM_SYSTEM TRUE)
elseif(EXISTS "/etc/os-release")
    file(READ "/etc/os-release" OS_RELEASE_CONTENT)
    if(OS_RELEASE_CONTENT MATCHES "(^|\n)(ID|ID_LIKE)=[\"']?[^\"'\n]*(debian|ubuntu)")
        set(IS_DEBIAN_SYSTEM TRUE)
    elseif(OS_RELEASE_CONTENT MATCHES "(^|\n)(ID|ID_LIKE)=[\"']?[^\"'\n]*(fedora|rhel|centos|suse|opensuse|almalinux|rocky|mageia|mandriva)")
        set(IS_RPM_SYSTEM TRUE)
    endif()
endif()

if(IS_DEBIAN_SYSTEM)
    set(CPACK_GENERATOR "DEB")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${CPACK_PACKAGE_CONTACT}")
    set(CPACK_DEBIAN_PACKAGE_SECTION "mail")
    set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "qt6-svg-plugins")
    set(CPACK_DEBIAN_FILE_NAME "DEB-DEFAULT")

    find_program(DPKG_CMD dpkg)
    if(DPKG_CMD)
        execute_process(COMMAND "${DPKG_CMD}" --print-architecture
            OUTPUT_VARIABLE CPACK_DEBIAN_PACKAGE_ARCHITECTURE
            OUTPUT_STRIP_TRAILING_WHITESPACE)
    else()
        message(FATAL_ERROR "dpkg not found (required for Debian packaging)")
    endif()
elseif(IS_RPM_SYSTEM)
    set(CPACK_GENERATOR "RPM")
    set(CPACK_RPM_PACKAGE_LICENSE "GPL-3.0-or-later")
    set(CPACK_RPM_PACKAGE_GROUP "Productivity/Networking/Email/Utilities")
    set(CPACK_RPM_PACKAGE_VENDOR "${CPACK_PACKAGE_CONTACT}")
    set(CPACK_RPM_PACKAGE_DESCRIPTION "${PROJECT_DESCRIPTION}")
    set(CPACK_RPM_PACKAGE_AUTOREQPROV ON)
    set(CPACK_RPM_FILE_NAME "RPM-DEFAULT")
    set(CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION
        "/usr/share/applications"
        "/usr/share/icons"
        "/usr/share/icons/hicolor"
        "/usr/share/icons/hicolor/64x64"
        "/usr/share/icons/hicolor/64x64/apps"
        "/usr/share/icons/hicolor/128x128"
        "/usr/share/icons/hicolor/128x128/apps"
    )

    find_program(RPMBUILD_CMD rpmbuild)
    if(RPMBUILD_CMD)
        execute_process(COMMAND "${RPMBUILD_CMD}" --eval "%{_arch}"
            OUTPUT_VARIABLE CPACK_RPM_PACKAGE_ARCHITECTURE
            OUTPUT_STRIP_TRAILING_WHITESPACE)
    else()
        message(FATAL_ERROR "rpmbuild not found (required for RPM packaging)")
    endif()
else()
    set(CPACK_GENERATOR "TGZ")
endif()

set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_IGNORE_FILES
    "/.git/"
    "/build/"
    "/cbuild/"
    "/\\\\.deps/"
    "\\\\.o$"
    "\\\\.a$"
    "\\\\.tar\\\\.gz$"
    "\\\\.rpm$"
    "\\\\.deb$"
    "/bin/"
)

include(CPack)


