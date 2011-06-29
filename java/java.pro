include(../config.pri)

NAME=java
VERSION=1

TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(java_plugin)
DEPENDPATH += src
INCLUDEPATH += src
DESTDIR = .

win32:TARGET = $$qtLibraryTarget(libjava_plugin)

CONFIG -= debug debug_and_release
CONFIG += release

LIBS += -lqscintilla2

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

##############################
# Install proper target file #
##############################

macx: java_TARGET_SOURCE = java.target.mac
unix:!macx: java_TARGET_SOURCE = java.target.linux
windows: java_TARGET_SOURCE = java.target.windows

java-target.target = java-target
java-target.depends = java-target-file

java-target-file.target = java.target
java-target-file.commands = cp $${java_TARGET_SOURCE} java.target
java-target-file.depends = java-target-source

java-target-source.target = $${java_TARGET_SOURCE}

QMAKE_EXTRA_TARGETS += java-target java-target-file java-target-source

POST_TARGETDEPS += java-target

QMAKE_CLEAN += java.target

######################
# Install Directives #
######################

GCC_TARGET_FILE_INSTALL = ../$${INSTALL_BASE}/targets/gcc/gcc.target

target_base.files = java.api template.c
target_base.path = ../$${INSTALL_BASE}/targets/java
target_base.extra = cp java.target $${GCC_TARGET_FILE_INSTALL}

target.path = ../$${INSTALL_BASE}/targets/java

INSTALLS += target target_base

include (../install.pri)