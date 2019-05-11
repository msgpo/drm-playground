add_executable(playground
    session/LogindSessionController.cc
    session/SessionController.cc

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
    NativeContext.cc
    NativeOutput.cc
    NativeRenderer.cc
    main.cc
)

target_link_libraries(playground
    Qt5::Core
    Qt5::DBus
    Qt5::Gui

    epoxy::epoxy
    gbm::gbm
    libdrm::libdrm
    libudev::libudev
)
