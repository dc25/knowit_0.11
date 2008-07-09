/***************************************************************************
                          knowitlinks.h  -  description
                             -------------------
    begin                : nie gru 21 2003
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

#ifndef __KNOWIT_LINKS_H__
#define __KNOWIT_LINKS_H__
#include <klistbox.h>

class QDropEvent;
class QKeyEvent;
class QDragEnterEvent;

class KnowitLinks : public KListBox
{
  Q_OBJECT
public:
  KnowitLinks(QWidget* parent=0, const char* name = 0);
protected:
  virtual void dragEnterEvent(QDragEnterEvent* event);
  virtual void dropEvent(QDropEvent* event);
  virtual void keyPressEvent(QKeyEvent* e);
signals:
  void textDropped(const QString& s, int flags);
  void linkRemove();
  void linkOpen();
  void linkOpenWith();
};

#endif

