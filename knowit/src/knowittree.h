/***************************************************************************
                          knowittree.h  -  description
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
#ifndef __KNOWIT_TREE_H__
#define __KNOWIT_TREE_H__
#include <klistview.h>
class QDragMoveEvent;
class KnowitTree : public KListView
{
  Q_OBJECT
public:
  KnowitTree(QWidget* parent=0, const char* name = 0);
  QListViewItem* prevItem;
  void takeNode(QListViewItem* item);
  void insertNode(QListViewItem* parent, QListViewItem* item, QListViewItem* after = 0);
protected:
  virtual void contentsDragMoveEvent(QDragMoveEvent* event);
public slots:  
  /** Shows only first-level notes */
  void slotCollapseAll();
  /** Shows only root of selected tree */
  void slotCollapse();
  /** Shows all notes */
  void slotExpandAll();
  /** Shows all notes of selected tree */
  void slotExpand();
  /** Move note up */
  void slotItemUp();
  /** Move note down */
  void slotItemDown();
  /** Move note at the beginning of current list */
  void slotItemBegin();
  /** Move note at the end of current list */
  void slotItemEnd();
  /** Move note level up */
  void slotItemLeft();
  /** Move note level down */
  void slotItemRight();
};

#endif

