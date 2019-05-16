add_executable(playground
    session/LogindSessionController.cc
    session/SessionController.cc

    udev/UdevContext.cc
    udev/UdevDevice.cc
    udev/UdevEnumerator.cc
    udev/UdevMonitor.cc

    Compositor.cc
    Scene.cc

    DrmAllocator.cc
    DrmBackend.cc
    DrmBlob.cc
    DrmBuffer.cc
    DrmConnector.cc
    DrmCrtc.cc
    DrmDevice.cc
    DrmDeviceManager.cc
    DrmDumbAllocator.cc
    DrmDumbImage.cc
    DrmGbmAllocator.cc
    DrmGbmImage.cc
    DrmImage.cc
    DrmMode.cc
    DrmObject.cc
    DrmOutput.cc
    DrmOutputManager.cc
    DrmPlane.cc
    DrmSwapchain.cc
    EDID.cc
    NativeContext.cc
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
