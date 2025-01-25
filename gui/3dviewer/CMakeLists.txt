cmake_minimum_required(VERSION 3.19)
project(3dviewer LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets OpenGLWidgets)

qt_standard_project_setup()

qt_add_executable(3dviewer
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    w_shader.vsh

    fshader.fsh
    ress.qrc
    player.cpp

)

target_link_libraries(3dviewer
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt6::OpenGLWidgets
)

include(GNUInstallDirs)

install(TARGETS 3dviewer
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET 3dviewer
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
