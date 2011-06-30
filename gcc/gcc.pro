include(../config.pri)

NAME=gcc
VER=1

TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(gcc_plugin)
DEPENDPATH += src
INCLUDEPATH += src
DESTDIR = .

win32:TARGET = $$qtLibraryTarget(libgcc_plugin)

LIBS += -lqscintilla2

HEADERS += 	Gcc.h Gdb.h
SOURCES += 	Gcc.cpp Gdb.cpp 

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

##############################
# Install proper target file #
##############################

macx: GCC_TARGET_SOURCE = gcc.target.mac
unix:!macx: GCC_TARGET_SOURCE = gcc.target.linux
windows: GCC_TARGET_SOURCE = gcc.target.windows

gcc-target.target = gcc-target
gcc-target.depends = gcc-target-file

gcc-target-file.target = gcc.target
gcc-target-file.commands = $${COPY} $${GCC_TARGET_SOURCE} gcc.target
gcc-target-file.depends = gcc-target-source

gcc-target-source.target = $${GCC_TARGET_SOURCE}

QMAKE_EXTRA_TARGETS += gcc-target gcc-target-file gcc-target-source

POST_TARGETDEPS += gcc-target

QMAKE_CLEAN += gcc.target

########
# glfw #
########

GLFW_LIB = ../external/lib/libglfw.a
GLFW_LIB_DEST = lib/libglfw.a
GLFW_HEADER = ../external/include/GL/glfw.h
GLFW_HEADER_DEST = include/GL/glfw.h

glfw.target = glfw
glfw.depends = glfw-lib glfw-header

glfw-lib.target = $${GLFW_LIB_DEST}
glfw-lib.commands = $${COPY} $${GLFW_LIB} $${GLFW_LIB_DEST}
glfw-lib.depends = $${GLFW_LIB}

glfw-header.target = $${GLFW_HEADER_DEST}
glfw-header.commands = $${COPY} $${GLFW_HEADER} $${GLFW_HEADER_DEST}
glfw-header.depends = $${GLFW_HEADER}

QMAKE_EXTRA_TARGETS += glfw glfw-lib glfw-header

POST_TARGETDEPS += glfw

QMAKE_CLEAN += $${GLFW_LIB_DEST} $${GLFW_HEADER_DEST}


########
# GLee #
########

GLEE_LIB = ../external/lib/libGLee.a
GLEE_LIB_DEST = lib/libGLee.a
GLEE_HEADER = ../external/include/GL/GLee.h
GLEE_HEADER_DEST = include/GL/GLee.h

glee.target = glee
glee.depends = glee-lib glee-header

glee-lib.target = $${GLEE_LIB_DEST}
glee-lib.commands = $${COPY} $${GLEE_LIB} $${GLEE_LIB_DEST}
glee-lib.depends = $${GLEE_LIB}

glee-header.target = $${GLEE_HEADER_DEST}
glee-header.commands = $${COPY} $${GLEE_HEADER} $${GLEE_HEADER_DEST}
glee-header.depends = $${GLEE_HEADER}

QMAKE_EXTRA_TARGETS += glee glee-lib glee-header

POST_TARGETDEPS += glee

QMAKE_CLEAN += $${GLEE_LIB_DEST} $${GLEE_HEADER_DEST}

###########
# libkiss #
###########

KISS_LIB = ../libraries/kiss/libkiss.a
KISS_LIB_DEST = lib/libkiss.a

KISS_HEADER_COMPAT = ../libraries/kiss/include/kiss-compat.h
KISS_HEADER_COMPAT_DEST = include/kiss-compat.h

KISS_HEADER_SERIAL = ../libraries/kiss/include/kiss-serial.h
KISS_HEADER_SERIAL_DEST = include/kiss-serial.h

KISS_HEADER_GRAPHICS = ../libraries/kiss/include/kiss-graphics.h
KISS_HEADER_GRAPHICS_DEST = include/kiss-graphics.h

KISS_HEADER_INPUT = ../libraries/kiss/include/kiss-input.h
KISS_HEADER_INPUT_DEST = include/kiss-input.h

kiss.target = kiss
kiss.depends = kiss-lib kiss-headers

kiss-headers.target = kiss-headers
kiss-headers.depends = 	kiss-header-compat \
						kiss-header-serial \
						kiss-header-graphics \
						kiss-header-input

kiss-lib.target = $${KISS_LIB_DEST}
kiss-lib.commands = $${COPY} $${KISS_LIB} $${KISS_LIB_DEST}
kiss-lib.depends = $${KISS_LIB}

kiss-header-compat.target = $${KISS_HEADER_COMPAT_DEST}
kiss-header-compat.commands = cp $${KISS_HEADER_COMPAT} $${KISS_HEADER_COMPAT_DEST}
kiss-header-compat.depends = $${KISS_HEADER_COMPAT}

kiss-header-serial.target = $${KISS_HEADER_SERIAL_DEST}
kiss-header-serial.commands = cp $${KISS_HEADER_SERIAL} $${KISS_HEADER_SERIAL_DEST}
kiss-header-serial.depends = $${KISS_HEADER_SERIAL}

kiss-header-graphics.target = $${KISS_HEADER_GRAPHICS_DEST}
kiss-header-graphics.commands = cp $${KISS_HEADER_GRAPHICS} $${KISS_HEADER_GRAPHICS_DEST}
kiss-header-graphics.depends = $${KISS_HEADER_GRAPHICS}

kiss-header-input.target = $${KISS_HEADER_INPUT_DEST}
kiss-header-input.commands = cp $${KISS_HEADER_INPUT} $${KISS_HEADER_INPUT_DEST}
kiss-header-input.depends = $${KISS_HEADER_INPUT}

QMAKE_EXTRA_TARGETS += kiss kiss-lib kiss-headers kiss-header-compat kiss-header-serial kiss-header-graphics kiss-header-input

POST_TARGETDEPS += kiss

QMAKE_CLEAN += 	$${KISS_LIB_DEST} \
				$${KISS_HEADER_COMPAT_DEST} \
				$${KISS_HEADER_SERIAL_DEST} \
				$${KISS_HEADER_GRAPHICS_DEST} \
				$${KISS_HEADER_INPUT_DEST}

##############
# Create Lib #
##############

CREATE_HEADER = ../libraries/create/kiss-create-2011.h
CREATE_HEADER_DEST = include/kiss-create-2011.h

create.target = create
create.depends = create-header

create-header.target = $${CREATE_HEADER_DEST}
create-header.commands = $${COPY} $${CREATE_HEADER} $${CREATE_HEADER_DEST}
create-header.depends = $${CREATE_HEADER}

QMAKE_EXTRA_TARGETS += create create-header

POST_TARGETDEPS += create

QMAKE_CLEAN += $${CREATE_HEADER_DEST}

######################
# Install Directives #
######################

GCC_TARGET_FILE_INSTALL = ../$${INSTALL_BASE}/targets/gcc/gcc.target

target_base.files = gcc.api template.c
target_base.path = ../$${INSTALL_BASE}/targets/gcc
target_base.extra = cp gcc.target $${GCC_TARGET_FILE_INSTALL}

target_include.files = ../libraries/kiss/include/kiss-compat.h \
					   ../libraries/kiss/include/kiss-input.h \
					   ../libraries/kiss/include/kiss-graphics.h \
					   ../libraries/kiss/include/kiss-serial.h \
					   ../libraries/create/kiss-create-2011.h

target_include.path = ../$${INSTALL_BASE}/targets/gcc/include

target_gl.files = ../external/include/GL/glfw.h ../external/include/GL/GLee.h
target_gl.path = ../$${INSTALL_BASE}/targets/gcc/include/GL

target_lib.files = ../external/lib/libglfw.a \
				   ../external/lib/libGLee.a \
				   ../libraries/kiss/libkiss.a
target_lib.path = ../$${INSTALL_BASE}/targets/gcc/lib

target.path = ../$${INSTALL_BASE}/targets/gcc

target_manual.files = 	manual/CBCCSS.css \
						manual/CBCCSS_NOSCRIPT.css \
						manual/gccmanual.html \
						manual/fdl-1.3.txt \
						manual/KISSCManualBody.htm \
						manual/KISSCManualTable.htm 
											
target_manual.path = ../$${INSTALL_BASE}/targets/gcc/manual

target_manual_images.files = manual/Images/box-minus.bmp \
							 manual/Images/box-null.GIF \
							 manual/Images/box-plus.bmp \
							 manual/Images/CBC_front_ports.jpg \
							 manual/Images/image002.gif \
							 manual/Images/image003.gif \
							 manual/Images/image004.gif \
							 manual/Images/image005.gif \
							 manual/Images/image006.gif \
							 manual/Images/image010.gif \
							 manual/Images/sonar.jpg

target_manual_images.path = ../$${INSTALL_BASE}/targets/gcc/manual/Images

INSTALLS += target target_base target_include target_gl target_lib

#################################
# Windows specifing MinGW stuff #
#################################

win32: {
	mingw.path = ../$${INSTALL_BASE}/targets/gcc
	mingw.extra = rm -Rf ../$${INSTALL_BASE}/targets/gcc/mingw; cp -r mingw ../$${INSTALL_BASE}/targets/gcc/mingw

	INSTALLS += mingw
}

include (../install.pri)