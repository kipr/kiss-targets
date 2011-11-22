include(../config.pri)

NAME=java
VER=1

TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(java_plugin)
DEPENDPATH += src
INCLUDEPATH += src
DESTDIR = .

win32:TARGET = $$qtLibraryTarget(libjava_plugin)

CONFIG -= debug debug_and_release
CONFIG += release
INCLUDEPATH += ../libraries/cbcserial

LIBS += -L../libraries/cbcserial -lcbcserial

HEADERS += Java.h
SOURCES += Java.cpp

QMAKE_CLEAN += $(DESTDIR)/$(TARGET) src/*~

#################
# Extra targets #
#################
lib_dir.target = lib
lib_dir.commands = mkdir -p lib


QMAKE_EXTRA_TARGETS += lib_dir include_dir
POST_TARGETDEPS += $$lib_dir.target $$include_dir.target

######################
# Install Directives #
######################

JAVA_TARGET_FILE_INSTALL = ../$${INSTALL_BASE}/targets/java/java.target

target_base.files = java.api template.c
target_base.path = ../$${INSTALL_BASE}/targets/java
target_base.extra = cp java.target $${JAVA_TARGET_FILE_INSTALL}
target_lib.files = lib/CBCJVM.jar
target_lib.path = ../$${INSTALL_BASE}/targets/java/lib

target.path = ../$${INSTALL_BASE}/targets/java

target_manual.files = 	manual/CBCJava_64.png \
						manual/index.html \
						manual/style.css
											
target_manual.path = ../$${INSTALL_BASE}/targets/java/manual

INSTALLS += target target_base target_lib target_manual

#############
# Templates #
#############

templates.path = ../$${INSTALL_BASE}/targets/java
templates.extra = rm -Rf ../$${INSTALL_BASE}/targets/java/templates; cp -r templates ../$${INSTALL_BASE}/targets/java/templates

INSTALLS += templates

include (../install.pri)