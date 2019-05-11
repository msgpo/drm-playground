#.rst:
# Findepoxy
# -------
#
# Try to find epoxy on a Unix system.
#
# This will define the following variables:
#
# ``epoxy_FOUND``
#     True if (the requested version of) epoxy is available
# ``epoxy_VERSION``
#     The version of epoxy
# ``epoxy_LIBRARIES``
#     This should be passed to target_compile_options() if the target is not
#     used for linking
# ``epoxy_INCLUDE_DIRS``
#     This should be passed to target_include_directories() if the target is not
#     used for linking
# ``epoxy_DEFINITIONS``
#     This should be passed to target_compile_options() if the target is not
#     used for linking
#
# If ``epoxy_FOUND`` is TRUE, it will also define the following imported target:
#
# ``epoxy::epoxy``
#     The epoxy library
#
# In general we recommend using the imported target, as it is easier to use.
# Bear in mind, however, that if the target is in the link interface of an
# exported library, it must be made available by the package config file.

find_package(PkgConfig)
pkg_check_modules(PKG_epoxy QUIET epoxy)

set(epoxy_VERSION ${PKG_epoxy_VERSION})
set(epoxy_DEFINITIONS ${PKG_epoxy_CFLAGS_OTHER})

find_path(epoxy_INCLUDE_DIRS
    NAMES epoxy/egl.h
    HINTS ${PKG_epoxy_INCLUDE_DIRS}
)

find_library(epoxy_LIBRARIES
    NAMES epoxy
    HINTS ${PKG_epoxy_LIBRARY_DIRS}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(epoxy
    FOUND_VAR epoxy_FOUND
    REQUIRED_VARS epoxy_LIBRARIES
                  epoxy_INCLUDE_DIRS
    VERSION_VAR epoxy_VERSION
)

if (epoxy_FOUND AND NOT TARGET epoxy::epoxy)
    add_library(epoxy::epoxy UNKNOWN IMPORTED)
    set_target_properties(epoxy::epoxy PROPERTIES
        IMPORTED_LOCATION "${epoxy_LIBRARIES}"
        INTERFACE_COMPILE_OPTIONS "${epoxy_DEFINITIONS}"
        INTERFACE_INCLUDE_DIRECTORIES "${epoxy_INCLUDE_DIRS}"
    )
endif()
