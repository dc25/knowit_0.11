/***************************************************************************
                          knowittray.h  -  description
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

#ifndef KNOWITTRAY_H
#define KNOWITTRAY_H
#include <ksystemtray.h>

class KnowitTray : public KSystemTray 
{
public:
   KnowitTray(QWidget* parent=0, const char* name=0);
protected:
   virtual void contextMenuAboutToShow (KPopupMenu* menu);
};

#endif

