/***************************************************************************
                          knowitApplication.cpp  -  description
                             -------------------
    begin                : wto maj 20 2003
    copyright            : (C) 2003 by Micha³ Rudolf
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

#include "knowitapp.h"

KnowitApplication::KnowitApplication(bool allowStyles, bool GUIEnabled)
      : KUniqueApplication(allowStyles, GUIEnabled, true), mcloseByLogout(false)
{
}

KnowitApplication::~KnowitApplication()
{
}

void KnowitApplication::commitData(QSessionManager& sm)
{
   mcloseByLogout = true;
   KUniqueApplication::commitData(sm);
}
