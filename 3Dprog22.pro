QT          += core gui widgets opengl

TEMPLATE    = app
CONFIG      += c++17

TARGET      = 3D-programmering

INCLUDEPATH += $$PWD/glm

SOURCES += main.cpp \
    bsplinecurve.cpp \
    camera.cpp \
    collisionvolume.cpp \
    cube.cpp \
    curves.cpp \
    directionallight.cpp \
    disc.cpp \
    interactivecollisionvolume.cpp \
    interactiveobject.cpp \
    light.cpp \
    logger.cpp \
    mainwindow.cpp \
    material.cpp \
    npc.cpp \
    octahedronball.cpp \
    plane.cpp \
    pointlight.cpp \
    quadtree.cpp \
    renderwindow.cpp \
    shader.cpp \
    tetrahedron.cpp \
    texture.cpp \
    trianglesurface.cpp \
    triangulation.cpp \
    vertex.cpp \
    visualobject.cpp \
    xyz.cpp

HEADERS += \
    CustomFiles/CommonValues.h \
    CustomFiles/CustomVec2.h \
    CustomFiles/CustomVec3.h \
    Vectors.h \
    bsplinecurve.h \
    camera.h \
    collisionvolume.h \
    cube.h \
    curves.h \
    directionallight.h \
    disc.h \
    interactivecollisionvolume.h \
    interactiveobject.h \
    light.h \
    logger.h \
    mainwindow.h \
    material.h \
    npc.h \
    octahedronball.h \
    plane.h \
    pointlight.h \
    quadtree.h \
    renderwindow.h \
    shader.h \
    tetrahedron.h \
    texture.h \
    trianglesurface.h \
    triangulation.h \
    vertex.h \
    visualobject.h \
    xyz.h

FORMS += \
    mainwindow.ui

DISTFILES += \
    phongshader.frag \
    phongshader.vert \
    plainshader.frag \
    plainshader.vert \
    texshader.frag \
    texshader.vert
