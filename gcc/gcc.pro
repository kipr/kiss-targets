include(../config.pri)

NAME=gcc
VER=1

TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(gcc_plugin)
DEPENDPATH += src
INCLUDEPATH += src ../libraries/gdb ../libraries/cbcserial
DESTDIR = .

win32:TARGET = $$qtLibraryTarget(libgcc_plugin)

LIBS += -L../libraries/gdb -L../libraries/cbcserial -lgdb -lcbcserial

HEADERS += Gcc.h
SOURCES += Gcc.cpp

QMAKE_CLEAN += $(DESTDIR)/$(TARGET) src/*~

#################
# Extra targets #
#################
COPY = cp

lib_dir.target = lib
lib_dir.commands = mkdir -p lib

include_dir.target = include
include_dir.commands = mkdir -p include/GL

QMAKE_EXTRA_TARGETS += lib_dir include_dir
POST_TARGETDEPS += $$lib_dir.target $$include_dir.target

######################
# Install Directives #
######################
GCC_TARGET_FILE_INSTALL = ../$${INSTALL_BASE}/targets/gcc/gcc.target

target_base.files = gcc.api
target_base.path = ../$${INSTALL_BASE}/targets/gcc
target_base.extra = cp gcc.target $${GCC_TARGET_FILE_INSTALL}

target_include.files = $${LIBKISS_INCLUDE_PATH}/kiss-compat.h \
	   $${LIBKISS_INCLUDE_PATH}/kiss-input.h \
	   $${LIBKISS_INCLUDE_PATH}/kiss-graphics.h \
	   $${LIBKISS_INCLUDE_PATH}/kiss-serial.h \
	   ../libraries/create/kiss-create-2011.h

target_include.path = ../$${INSTALL_BASE}/targets/gcc/include

target_gl.files = $${GL_INCLUDE_PATH}/GL/glfw.h $${GL_INCLUDE_PATH}/GL/GLee.h
target_gl.path = ../$${INSTALL_BASE}/targets/gcc/include/GL

target_lib.files = $${GL_LIB_PATH}/libglfw.a $${GL_LIB_PATH}/libGLee.a $${LIBKISS_LIB_PATH}/libkiss.a
target_lib.path = ../$${INSTALL_BASE}/targets/gcc/lib

target.path = ../$${INSTALL_BASE}/targets/gcc

# Manual
MANUAL_LOCATION = ../$${INSTALL_BASE}/targets/gcc/manual

manual.path = ../$${INSTALL_BASE}/targets/gcc
manual.extra = rm -Rf $${MANUAL_LOCATION}; $${COPY} -r $${KISS_DOCS}/gcc_manual $${MANUAL_LOCATION}

INSTALLS += target target_base target_include target_gl target_lib manual

#################################
# Windows specifing MinGW stuff #
#################################
win32: {
	mingw.path = ../$${INSTALL_BASE}/targets/gcc
	mingw.extra = rm -Rf ../$${INSTALL_BASE}/targets/gcc/mingw; cp -r mingw ../$${INSTALL_BASE}/targets/gcc/mingw

	INSTALLS += mingw
}

#############
# Templates #
#############

templates.path = ../$${INSTALL_BASE}/targets/gcc
templates.extra = rm -Rf ../$${INSTALL_BASE}/targets/gcc/templates; cp -r templates ../$${INSTALL_BASE}/targets/gcc/templates

INSTALLS += templates

include (../install.pri)