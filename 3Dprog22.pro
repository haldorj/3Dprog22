QT          += core gui widgets opengl

TEMPLATE    = app
CONFIG      += c++17

TARGET      = 3D-programmering

SOURCES += main.cpp \
    camera.cpp \
    cube.cpp \
    curves.cpp \
    disc.cpp \
    interactiveobject.cpp \
    logger.cpp \
    mainwindow.cpp \
    octahedronball.cpp \
    quadtree.cpp \
    renderwindow.cpp \
    shader.cpp \
    tetrahedron.cpp \
    trianglesurface.cpp \
    vertex.cpp \
    visualobject.cpp \
    xyz.cpp

HEADERS += \
    camera.h \
    cube.h \
    curves.h \
    disc.h \
    interactiveobject.h \
    logger.h \
    mainwindow.h \
    octahedronball.h \
    quadtree.h \
    renderwindow.h \
    shader.h \
    tetrahedron.h \
    trianglesurface.h \
    vertex.h \
    visualobject.h \
    xyz.h

FORMS += \
    mainwindow.ui

DISTFILES += \
    plainshader.frag \
    plainshader.vert
