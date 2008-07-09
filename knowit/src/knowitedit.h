/***************************************************************************
                          knowitedit.h  -  description
                             -------------------
    begin                : pi± maj 23 2003
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
#ifndef __KNOWIT_EDIT_H__
#define __KNOWIT_EDIT_H__
#include <ktextedit.h>

class KnowitEdit : public KTextEdit
{
  Q_OBJECT
public:
   bool swapEOL;
   KnowitEdit(const QString& text, const QString& context=QString::null,
      QWidget* parent=0, const char* name = 0);
   KnowitEdit(QWidget* parent=0, const char* name = 0);
   virtual void keyPressEvent(QKeyEvent* e);
   virtual void contentsDragEnterEvent(QDragEnterEvent* event);
   virtual void contentsDropEvent(QDropEvent* event);
signals:
   void textDropped(const QString& s, int flags);
};


#endif

