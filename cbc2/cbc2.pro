include(../config.pri)

NAME=cbc2
VER=1

TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(cbc2_plugin)
DEPENDPATH += src
INCLUDEPATH += src ../libraries/gdb ../libraries/cbcserial
DESTDIR = .

win32:TARGET = $$qtLibraryTarget(libcbc2_plugin)

LIBS += -L../libraries/gdb -L../libraries/cbcserial -lgdb -lcbcserial

HEADERS += CBC.h Controller.h
SOURCES += CBC.cpp Controller.cpp
FORMS += Controller.ui
RESOURCES += rc/rc.qrc

QMAKE_CLEAN += $(DESTDIR)/$(TARGET) src/*~

#################
# Extra targets #
#################
COPY = cp

lib_dir.target = lib
lib_dir.commands = mkdir -p lib
include_dir.target = include
include_dir.commands = mkdir -p include

QMAKE_EXTRA_TARGETS += lib_dir include_dir
POST_TARGETDEPS += $$lib_dir.target $$include_dir.target

######################
# Install Directives #
######################

CBC_TARGET_FILE_INSTALL = ../$${INSTALL_BASE}/targets/cbc2/cbc2.target

target_base.files = cbc2.api
target_base.path = ../$${INSTALL_BASE}/targets/cbc2
target_base.extra = $${COPY} cbc2.target $${CBC_TARGET_FILE_INSTALL}

target_include.files = ../libraries/simulator/cbc2_sim/include/cbc2-sim.h ../libraries/simulator/cbc2_sim/include/cbc2cxx.h ../../libraries/simulator/cbc2_sim/src/botball.c

target_lib.files = ../libraries/simulator/cbc2_sim/libcbc2_sim.a


target_include.path = ../$${INSTALL_BASE}/targets/cbc2/include
target_lib.path = ../$${INSTALL_BASE}/targets/cbc2/lib

target.path = ../$${INSTALL_BASE}/targets/cbc2

target_manual.files = 	manual/CBCCSS.css \
						manual/CBCCSS_NOSCRIPT.css \
						manual/cbcmanual.html \
						manual/fdl-1.3.txt \
						manual/KISSCBCManualBody.htm \
						manual/KISSCBCManualTable.htm \
						manual/Sensor_and_Motor_Manual_BB2011.pdf
											
target_manual.path = ../$${INSTALL_BASE}/targets/cbc2/manual

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

target_manual_images.path = ../$${INSTALL_BASE}/targets/cbc2/manual/Images


INSTALLS += target target_base target_include target_lib target_manual target_manual_images

#################
# cbc2_sim stuff #
#################

CBC2_SIM_LIB = ../libraries/simulator/cbc2_sim/libcbc2_sim.a
CBC2_SIM_LIB_DEST = lib/libcbc2_sim.a

CBC2_SIM_HEADER = ../libraries/simulator/cbc2_sim/include/cbc2-sim.h
CBC2_SIM_HEADER_DEST = include/cbc2-sim.h

CBC2CXX_HEADER = ../libraries/simulator/cbc2_sim/include/cbc2cxx.h
CBC2CXX_HEADER_DEST = include/cbc2cxx.h

cbc2_sim.target = cbc2_sim
cbc2_sim.depends = cbc2_sim-lib cbc2_sim-headers

cbc2_sim-headers.target = cbc2_sim-headers
cbc2_sim-headers.depends = cbc2_sim-header cbc2cxx-header

cbc2_sim-lib.target = $${CBC2_SIM_LIB_DEST}
cbc2_sim-lib.commands = $${COPY} $${CBC2_SIM_LIB} $${CBC2_SIM_LIB_DEST}
cbc2_sim-lib.depends = $${CBC2_SIM_LIB}

cbc2_sim-header.target = $${CBC2_SIM_HEADER_DEST}
cbc2_sim-header.commands = $${COPY} $${CBC2_SIM_HEADER} $${CBC2_SIM_HEADER_DEST}
cbc2_sim-header.depends = $${CBC2_SIM_HEADER}

cbc2cxx-header.target = $${CBC2CXX_HEADER_DEST}
cbc2cxx-header.commands = $${COPY} $${CBC2CXX_HEADER} $${CBC2CXX_HEADER_DEST}
cbc2cxx-header.depends = $${CBC2CXX_HEADER}

QMAKE_EXTRA_TARGETS += cbc2_sim cbc2_sim-lib cbc2_sim-headers cbc2_sim-header cbc2cxx-header

POST_TARGETDEPS += cbc2_sim

QMAKE_CLEAN += $${CBC2_SIM_LIB_DEST} $${CBC2_SIM_HEADER_DEST}

#############
# Templates #
#############

templates.path = ../$${INSTALL_BASE}/targets/cbc2
templates.extra = rm -Rf ../$${INSTALL_BASE}/targets/cbc2/templates; cp -r templates ../$${INSTALL_BASE}/targets/cbc2/templates

INSTALLS += templates

include (../install.pri)
