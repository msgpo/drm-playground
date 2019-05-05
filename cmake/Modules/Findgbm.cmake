#.rst:
# Findgbm
# -------
#
# Try to find gbm on a Unix system.
#
# This will define the following variables:
#
# ``gbm_FOUND``
#     True if (the requested version of) gbm is available
# ``gbm_VERSION``
#     The version of gbm
# ``gbm_LIBRARIES``
#     This should be passed to target_compile_options() if the target is not
#     used for linking
# ``gbm_INCLUDE_DIRS``
#     This should be passed to target_include_directories() if the target is not
#     used for linking
# ``gbm_DEFINITIONS``
#     This should be passed to target_compile_options() if the target is not
#     used for linking
#
# If ``gbm_FOUND`` is TRUE, it will also define the following imported target:
#
# ``gbm::gbm``
#     The gbm library
#
# In general we recommend using the imported target, as it is easier to use.
# Bear in mind, however, that if the target is in the link interface of an
# exported library, it must be made available by the package config file.

find_package(PkgConfig)
pkg_check_modules(PKG_gbm QUIET gbm)

set(gbm_VERSION ${PKG_gbm_VERSION})
set(gbm_DEFINITIONS ${PKG_gbm_CFLAGS_OTHER})

find_path(gbm_INCLUDE_DIRS
    NAMES gbm.h
    HINTS ${PKG_gbm_INCLUDE_DIRS}
)

find_library(gbm_LIBRARIES
    NAMES gbm
    HINTS ${PKG_gbm_LIBRARY_DIRS}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(gbm
    FOUND_VAR gbm_FOUND
    REQUIRED_VARS gbm_LIBRARIES
                  gbm_INCLUDE_DIRS
    VERSION_VAR gbm_VERSION
)

if (gbm_FOUND AND NOT TARGET gbm::gbm)
    add_library(gbm::gbm UNKNOWN IMPORTED)
    set_target_properties(gbm::gbm PROPERTIES
        IMPORTED_LOCATION "${gbm_LIBRARIES}"
        INTERFACE_COMPILE_OPTIONS "${gbm_DEFINITIONS}"
        INTERFACE_INCLUDE_DIRECTORIES "${gbm_INCLUDE_DIRS}"
    )
endif()
