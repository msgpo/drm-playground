add_executable(playground
    udev/UdevContext.cc
    udev/UdevDevice.cc
    udev/UdevEnumerator.cc
    udev/UdevMonitor.cc

    DrmBackend.cc
    DrmBuffer.cc
    DrmConnector.cc
    DrmCrtc.cc
    DrmDevice.cc
    DrmDeviceManager.cc
    DrmMode.cc
    DrmObject.cc
    DrmPlane.cc
    EDID.cc
    main.cc
)

target_link_libraries(playground
    Qt5::Core
    Qt5::Gui

    gbm::gbm
    libdrm::libdrm
    libudev::libudev

    ${epoxy_LIBRARY}
)
