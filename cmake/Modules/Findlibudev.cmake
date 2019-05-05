#.rst:
# Findlibudev
# -------
#
# Try to find libudev on a Unix system.
#
# This will define the following variables:
#
# ``libudev_FOUND``
#     True if (the requested version of) libudev is available
# ``libudev_VERSION``
#     The version of libudev
# ``libudev_LIBRARIES``
#     This should be passed to target_compile_options() if the target is not
#     used for linking
# ``libudev_INCLUDE_DIRS``
#     This should be passed to target_include_directories() if the target is not
#     used for linking
# ``libudev_DEFINITIONS``
#     This should be passed to target_compile_options() if the target is not
#     used for linking
#
# If ``libudev_FOUND`` is TRUE, it will also define the following imported target:
#
# ``libudev::libudev``
#     The libudev library
#
# In general we recommend using the imported target, as it is easier to use.
# Bear in mind, however, that if the target is in the link interface of an
# exported library, it must be made available by the package config file.

find_package(PkgConfig)
pkg_check_modules(PKG_libudev QUIET libudev)

set(libudev_VERSION ${PKG_libudev_VERSION})
set(libudev_DEFINITIONS ${PKG_libudev_CFLAGS_OTHER})

find_path(libudev_INCLUDE_DIRS
    NAMES libudev.h
    HINTS ${PKG_libudev_INCLUDE_DIRS}
)

find_library(libudev_LIBRARIES
    NAMES udev
    HINTS ${PKG_libudev_LIBRARY_DIRS}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(libudev
    FOUND_VAR libudev_FOUND
    REQUIRED_VARS libudev_LIBRARIES
                  libudev_INCLUDE_DIRS
    VERSION_VAR libudev_VERSION
)

if (libudev_FOUND AND NOT TARGET libudev::libudev)
    add_library(libudev::libudev UNKNOWN IMPORTED)
    set_target_properties(libudev::libudev PROPERTIES
        IMPORTED_LOCATION "${libudev_LIBRARIES}"
        INTERFACE_COMPILE_OPTIONS "${libudev_DEFINITIONS}"
        INTERFACE_INCLUDE_DIRECTORIES "${libudev_INCLUDE_DIRS}"
    )
endif()
