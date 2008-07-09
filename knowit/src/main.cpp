/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : czw wrz 26 08:27:40 CEST 2002
    copyright            : (C) 2002-2004 by Micha³ Rudolf
    email                : mrudolf@kdewebdev.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>
#include "knowit.h"
#include "knowitapp.h"

static const char *description =
  I18N_NOOP("Kommander is a graphical tool to create scripted dialogs.");


static KCmdLineOptions options[] =
{
  { "+file", I18N_NOOP("Document to open"), 0 },
  { 0, 0, 0 }
};

int main(int argc, char *argv[])
{
  KAboutData aboutData( "knowit", I18N_NOOP("KnowIt"),
    VERSION, description, KAboutData::License_GPL,
    "(C) 2002-2004 Michal Rudolf", 0, 0, "mrudolf@kdewebdev.org");
  aboutData.addAuthor("Michal Rudolf",0, "mrudolf@kdewebdev.org");
  aboutData.addAuthor("Michal Rudolf",0, "mrudolf@kdewebdev.org");
  
  aboutData.setTranslator (I18N_NOOP("_: NAME OF TRANSLATORS\nYour names"),
     I18N_NOOP("_: EMAIL OF TRANSLATORS\nYour emails"));
  
  KCmdLineArgs::init( argc, argv, &aboutData );
  KCmdLineArgs::addCmdLineOptions(options); 

  KnowitApplication a(true, true);

  Knowit *knowit = new Knowit(0, "KnowIt");
  a.setMainWidget(knowit);
  return a.exec();
}
