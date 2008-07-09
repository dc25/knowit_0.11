/***************************************************************************
                          knowittray.cpp  -  description
                             -------------------
    begin                : czw lis 28 2002
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
#include "knowittray.h"
#include <kpopupmenu.h>
#include <klocale.h>
  
KnowitTray::KnowitTray (QWidget* parent, const char* name) :
   KSystemTray(parent, name)
{
   contextMenu()->insertItem(i18n("&Undock"), parent,
      SLOT(slotUndock()));
}

void KnowitTray::contextMenuAboutToShow (KPopupMenu* menu)
{
   menu->removeItemAt(menu->count()-1);
   menu->insertItem(i18n("&Quit"), parent(), SLOT(slotFileQuit()));
}

