/***************************************************************************
                          knowitlinks.cpp  -  description
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

#include "knowitlinks.h"
#include <qevent.h>
#include <kurldrag.h>

KnowitLinks::KnowitLinks(QWidget* parent, const char* name)
   : KListBox(parent, name)
{
   setAcceptDrops(true);
}

void KnowitLinks::dragEnterEvent(QDragEnterEvent* event)
{
   event->accept(KURLDrag::canDecode(event));
}

void KnowitLinks::dropEvent(QDropEvent* event)
{
   if (KURLDrag::canDecode(event)) {
      KURL::List L;
      KURLDrag::decode(event, L);
      for (uint i=0; i<L.count(); i++)
         emit textDropped(L[i].url(), L.count() > 1);
   }
}

void KnowitLinks::keyPressEvent(QKeyEvent* e)
{
   if (currentItem() != -1 && (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)) {
      e->ignore();
      if ((e->state() | Qt::NoButton | Qt::Keypad) ==
         (Qt::NoButton | Qt::Keypad)) emit linkOpen();
      else emit linkOpenWith();
      }
   else if (currentItem() != -1 && e->key() == Qt::Key_Delete) {
      e->ignore();
      emit linkRemove();
      }
   else KListBox::keyPressEvent(e);
}
