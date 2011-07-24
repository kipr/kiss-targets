#############################
# OS X Specific dylib stuff #
#############################

macx:{
	install_lib_names.extra = sh $${KISS}/scripts/osx_install_names.sh ../$${INSTALL_BASE}/targets/$${NAME}/lib$${NAME}_plugin.dylib QtCore.framework/Versions/4/QtCore QtGui.framework/Versions/4/QtGui
	install_lib_names.path = ../$${INSTALL_BASE}

	INSTALLS += install_lib_names
}

#############################
# Build Package for Release #
#############################

macx: create_kiss_archives.extra = cd ../root; \
	find targets/$${NAME} -type f > contents; \
	$${KISS}/deploy/KISS.app/Contents/MacOS/KISS --createArchive $${NAME} $${VER} osx contents $${NAME}_target_osx.kiss; \
	rm contents; \
	cd - > /dev/null

unix:!macx: create_kiss_archives.extra = cd ../root; \
	find targets/$${NAME} -type f > contents; \
	$${KISS}/deploy/KISS --createArchive $${NAME} $${VER} nix contents $${NAME}_target_nix.kiss; \
	rm contents; \
	cd - > /dev/null
		
win32: create_kiss_archives.extra = cd ../root; \
	find targets/$${NAME} -type f > contents; \
	$${KISS}/deploy/KISS --createArchive $${NAME} $${VER} win contents $${NAME}_target_win.kiss; \
	rm contents; \
	cd - > /dev/null

create_kiss_archives.path = ../root

INSTALLS += create_kiss_archives