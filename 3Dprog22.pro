QT          += core gui widgets opengl

TEMPLATE    = app
CONFIG      += c++17

TARGET      = 3D-programmering

SOURCES += main.cpp \
    camera.cpp \
    collisionvolume.cpp \
    cube.cpp \
    curves.cpp \
    disc.cpp \
    interactivecollisionvolume.cpp \
    interactiveobject.cpp \
    logger.cpp \
    mainwindow.cpp \
    npc.cpp \
    octahedronball.cpp \
    plane.cpp \
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
    collisionvolume.h \
    cube.h \
    curves.h \
    disc.h \
    interactivecollisionvolume.h \
    interactiveobject.h \
    logger.h \
    mainwindow.h \
    npc.h \
    octahedronball.h \
    plane.h \
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
