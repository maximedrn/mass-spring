include(FetchContent)

if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

set(FETCHCONTENT_TRY_FIND_PACKAGE_MODE NEVER)
FetchContent_Declare(libqglviewer
    # libQGLViewer 2.9.1, pinned to its release commit.
    URL https://codeload.github.com/GillesDebunne/libQGLViewer/tar.gz/ba9a875784afbb7ee73088fe0e8701c31bc7277d
    URL_HASH SHA256=0d9ca8466683b83bdf655efd84722ffb4effb0acd93971470bea4797534c9cfb
    TLS_VERIFY ON
    # Skip upstream CMake, which also builds examples and prefers Qt 6.
    SOURCE_SUBDIR QGLViewer
)
FetchContent_MakeAvailable(libqglviewer)

file(GLOB QGLVIEWER_SOURCES
    "${libqglviewer_SOURCE_DIR}/QGLViewer/*.cpp"
    "${libqglviewer_SOURCE_DIR}/QGLViewer/VRender/*.cpp"
)
add_library(qglviewer STATIC ${QGLVIEWER_SOURCES})
add_library(QGLViewer::QGLViewer ALIAS qglviewer)
set_target_properties(qglviewer PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    ARCHIVE_OUTPUT_DIRECTORY "${libqglviewer_BINARY_DIR}"
)
target_include_directories(qglviewer SYSTEM PUBLIC "${libqglviewer_SOURCE_DIR}")
target_compile_definitions(qglviewer
    PUBLIC QGLVIEWER_STATIC
    PRIVATE QT_NO_KEYWORDS "$<$<PLATFORM_ID:Windows>:WIN32;NOMINMAX>"
)
target_link_libraries(qglviewer PUBLIC Qt5::Widgets Qt5::OpenGL Qt5::Xml OpenGL::GL)
if(TARGET OpenGL::GLU)
    target_link_libraries(qglviewer PUBLIC OpenGL::GLU)
endif()
