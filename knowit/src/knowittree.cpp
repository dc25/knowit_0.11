/***************************************************************************
                          knowittree.cpp  -  description
                             -------------------
    begin                : sob gru 20 2003
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

#include <klocale.h>
#include <qevent.h>
#include <qdragobject.h>
#include <qheader.h>
#include <qwhatsthis.h>
#include "knowittree.h"

KnowitTree::KnowitTree(QWidget* parent, const char* name)
   : KListView(parent, name)
{
   setMinimumSize(150, 150);
   addColumn(i18n("Notes"));
   setRootIsDecorated(true);
   setItemsRenameable(true);
   setDragEnabled(true);
   setAcceptDrops(true);
   setFullWidth(true);
   setSorting(-1);
   setDragAutoScroll(true);
   prevItem = 0;
   QWhatsThis::add(this, i18n("<h2>Notes hierarchy</h2>"
      "Here you can browse notes tree. "
      "You can also add notes using right-click menu, or reorganize them using "
      "drag and drop."));
}


void KnowitTree::takeNode(QListViewItem* item)
{
   if (item->parent())
      item->parent()->takeItem(item);
   else
      takeItem(item);
}

void KnowitTree::insertNode(QListViewItem* parent, QListViewItem* item, QListViewItem* after)
{
   if (parent)
      parent->insertItem(item);
   else
      insertItem(item);
   if (after)
      item->moveItem(after);
}


void KnowitTree::contentsDragMoveEvent(QDragMoveEvent* event)
{
   QListViewItem* item = itemAt(event->pos());
   /* check if on root decoration */
   if (!item || event->pos().x() > header()->cellPos(header()->mapToActual(0)) +
         treeStepSize() * (item->depth() + 1) + itemMargin() ||
      event->pos().x() < header()->cellPos(header()->mapToActual(0)))
      return;
   if (item && item->childCount() && !item->isOpen())
      item->setOpen(true);
}

void KnowitTree::slotCollapseAll()
{
   QListViewItem* it = firstChild();
   while(it) {
      it->setOpen(false);
      it = it->nextSibling();
      }
}

void KnowitTree::slotCollapse()
{
   if (currentItem())
      currentItem()->setOpen(false);
}


void KnowitTree::slotExpandAll()
{
   QListViewItem* it = firstChild();
   while(it) {
      it->setOpen(true);
      it = it->nextSibling();
      }
}

void KnowitTree::slotExpand()
{
   if (currentItem())
      currentItem()->setOpen(true);
}

void KnowitTree::slotItemUp()
{
   if (currentItem())
      setCurrentItem(currentItem()->itemAbove());
}

void KnowitTree::slotItemDown()
{
   if (currentItem())
      setCurrentItem(currentItem()->itemBelow());
}

void KnowitTree::slotItemBegin()
{
   setCurrentItem(firstChild());
}

void KnowitTree::slotItemEnd()
{
   QListViewItem* elt = firstChild();
   if (elt)
      while (elt->itemBelow())
         elt = elt->itemBelow();
   setCurrentItem(elt);
}

void KnowitTree::slotItemLeft()
{
   if (currentItem())
      setCurrentItem(currentItem()->parent());
}

void KnowitTree::slotItemRight()
{
   if (currentItem())
      setCurrentItem(currentItem()->firstChild());
}



