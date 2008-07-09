# set the include path for X, qt and KDE
INCLUDES = $(all_includes)

# these are the headers for your project
noinst_HEADERS = knowit.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(EXTRACTRC) `find . -name \*.ui -o -name \*.rc` > rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/knowit.pot

KDE_ICON = AUTO

#########################################################################
# APPLICATION SECTION
#########################################################################
# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = knowit

# the application source, library search path, and link libraries
knowit_SOURCES = main.cpp knowit.cpp knowitapp.cpp knowitchooser.cpp \
   knowitedit.cpp  knowitlink.cpp knowitlinks.cpp knowitpref.cpp \
   knowittray.cpp knowittree.cpp notes.cpp
knowit_LDFLAGS = $(KDE_RPATH) $(all_libraries)
knowit_LDADD = $(LIB_KDEPRINT) $(LIB_KDEUI)

# this is where the desktop file will go 
shelldesktopdir   = $(kde_appsdir)/Utilities
shelldesktop_DATA = knowit.desktop

# this is where the shell's XML-GUI resource file goes
shellrcdir   = $(kde_datadir)/knowit
shellrc_DATA = knowitui.rc

