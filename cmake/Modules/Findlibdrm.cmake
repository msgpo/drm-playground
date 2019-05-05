#.rst:
# Findlibdrm
# -------
#
# Try to find libdrm on a Unix system.
#
# This will define the following variables:
#
# ``libdrm_FOUND``
#     True if (the requested version of) libdrm is available
# ``libdrm_VERSION``
#     The version of libdrm
# ``libdrm_LIBRARIES``
#     This should be passed to target_compile_options() if the target is not
#     used for linking
# ``libdrm_INCLUDE_DIRS``
#     This should be passed to target_include_directories() if the target is not
#     used for linking
# ``libdrm_DEFINITIONS``
#     This should be passed to target_compile_options() if the target is not
#     used for linking
#
# If ``libdrm_FOUND`` is TRUE, it will also define the following imported target:
#
# ``libdrm::libdrm``
#     The libdrm library
#
# In general we recommend using the imported target, as it is easier to use.
# Bear in mind, however, that if the target is in the link interface of an
# exported library, it must be made available by the package config file.

find_package(PkgConfig)
pkg_check_modules(PKG_libdrm QUIET libdrm)

set(libdrm_VERSION ${PKG_libdrm_VERSION})
set(libdrm_DEFINITIONS ${PKG_libdrm_CFLAGS_OTHER})

find_path(libdrm_INCLUDE_DIRS
    NAMES drm.h xf86drm.h
    HINTS ${PKG_libdrm_INCLUDE_DIRS}
)

find_library(libdrm_LIBRARIES
    NAMES drm
    HINTS ${PKG_libdrm_LIBRARY_DIRS}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(libdrm
    FOUND_VAR libdrm_FOUND
    REQUIRED_VARS libdrm_LIBRARIES
                  libdrm_INCLUDE_DIRS
    VERSION_VAR libdrm_VERSION
)

if (libdrm_FOUND AND NOT TARGET libdrm::libdrm)
    add_library(libdrm::libdrm UNKNOWN IMPORTED)
    set_target_properties(libdrm::libdrm PROPERTIES
        IMPORTED_LOCATION "${libdrm_LIBRARIES}"
        INTERFACE_COMPILE_OPTIONS "${libdrm_DEFINITIONS}"
        INTERFACE_INCLUDE_DIRECTORIES "${libdrm_INCLUDE_DIRS}"
    )
endif()
