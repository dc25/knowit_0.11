/***************************************************************************
                          knowitedit.cpp  -  description
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

#include "knowitedit.h"
#include <kurldrag.h>

KnowitEdit::KnowitEdit(const QString& text, const QString& context, QWidget* parent,
   const char* name) : KTextEdit(text, context, parent, name), swapEOL(true)
{
   setAcceptDrops(true);
}

      
KnowitEdit::KnowitEdit(QWidget* parent, const char* name) : KTextEdit(parent, name),
   swapEOL(true)
{

}

void KnowitEdit::keyPressEvent(QKeyEvent* e)
{
   if (swapEOL &&
          (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return) &&
          (e->state() | (Qt::NoButton |  Qt::ControlButton | Qt::Keypad) ==
             (Qt::NoButton |  Qt::ControlButton | Qt::Keypad))) {
      QKeyEvent* e1 = new QKeyEvent(QKeyEvent::KeyPress, e->key(),
         e->ascii(), e->state() ^ Qt::ControlButton, e->text(), e->isAutoRepeat(),
         e->count());
      e->ignore();
      QTextEdit::keyPressEvent(e1);      
      }
   else QTextEdit::keyPressEvent(e);
}

void KnowitEdit::contentsDragEnterEvent(QDragEnterEvent* event)
{
   if (KURLDrag::canDecode(event))
      event->accept(true);
   else QTextEdit::contentsDragEnterEvent(event);
}

void KnowitEdit::contentsDropEvent(QDropEvent* event)
{
   if (KURLDrag::canDecode(event)) {
      KURL::List L;
      KURLDrag::decode(event, L);
      for (uint i=0; i<L.count(); i++)
         emit textDropped(L[i].url(), L.count() > 1);
   }
   else QTextEdit::contentsDropEvent(event);
}

