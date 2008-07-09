/***************************************************************************
                          knowitApplication.h  -  description
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

#ifndef __KNOWIT_APP_H__
#define __KNOWIT_APP_H__
#include <kuniqueapplication.h>

class KnowitApplication : public KUniqueApplication
{
Q_OBJECT

public:
  KnowitApplication(bool allowStyles = true, bool GUIEnabled = true);
  ~KnowitApplication();
  /** Commits all Application-relevant data on shutdown.
    *
    * This function is called when the QSessionManager wants to close the
    * Application. It is reimplemented to set the boolean sessionClose flag.
    *
    * @param sm  The QSessionManager object.
    */
  virtual void commitData(QSessionManager& sm);
  /** Returns TRUE if the Application is closed by the session manager. */
  bool closeByLogout() {return(mcloseByLogout);}

private:
  bool mcloseByLogout;
};

#endif

