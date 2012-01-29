You will need a clone of the kiss repository to build this one.
to build:

	cd kiss-targets
	
	touch kiss.pri

You will need to populate kiss.pri with the following definitions: (Included is my kiss.pri, you'll need to modify the directories.)

	KISS=/Volumes/Eve/Development/kiss
	GL_LIB_PATH=/Volumes/Eve/lib
	GL_INCLUDE_PATH=/Volumes/Eve/include
	LIBKISS_LIB_PATH=/usr/local/lib
	LIBKISS_INCLUDE_PATH=/usr/local/include
	KISS_DOCS=/Volumes/Eve/Development/kiss-docs

The GL definitions must include both GLFW and GLee. libkiss should be downloaded from the libkiss git repository.

	qmake -r
	make clean
	make
	make install

The kiss archives will be generated in the root directory, which can be installed into kiss using File, Install Local Packages.	


