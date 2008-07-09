/***************************************************************************
                          notes.h  -  description
                             -------------------
    begin                : sob lis 16 2002
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
#ifndef KNOWIT_NOTES_H
#define KNOWIT_NOTES_H

#include <qstring.h>
#include <qpixmap.h>
#include <qvaluevector.h>
#include <qvaluelist.h>
#include <qptrdict.h>

class QTextStream;
class QListViewItem;
class KURL;

class TNote;

class TNoteLink
{
public:
   enum {DescriptionLink, LinkDescription, LinkOnly, DescriptionOnly};
   QString link;
   QString description;
   TNoteLink()   {}
   TNoteLink(const QString& s)   {link = s;}
   TNoteLink(const QString& s1, const QString& s2) {link = s1; description = s2;}
   int operator==(const TNoteLink& N) const   {return link == N.link &&
      description == N.description;}
   int operator!=(const TNoteLink& N) const   {return link != N.link ||
      description != N.description;}
   void save(QTextStream& ts) const;
   void open() const;
   void openWith() const;
   QString text(int fmt = DescriptionLink) const;
   QPixmap icon() const;
   bool isLocalReference() const;
};


class TNotesCollection : public QPtrDict<TNote> {
public:
   QPixmap Pixmaps[4];
   TNotesCollection();
   ~TNotesCollection();
   void addNote(QListViewItem* item);
   void addNote(QListViewItem* item, const QString& s);
   void removeNote(QListViewItem* item);
   bool changeNote(QListViewItem* item, const QString& s);
   void clearNotes();
   void updateNotesView();
   bool modified;
   bool autoUpdate;
   const QString& text(QListViewItem* item);
   static int wordCount(const QString& text);
   static int charCount(const QString& text);
   static int parsCount(const QString& text);
};


class TNote {
private:
   QValueList<TNoteLink> links;
   TNotesCollection* collection;
   bool saveHTMLBuf(QTextStream& ts, QValueVector<int>& depths,
      int flags = SaveAll);
   bool saveHTMLTocEntry(QTextStream& ts, QValueVector<int>& depths, int flags);
public:
   typedef enum {Empty, Text, EmptyParent, TextParent} State;
   enum {SaveCurrent = 0, SaveSubnotes = 0x0001, SaveAll = 0x0002, UseUTF8 = 0x0040,
         Enumerate = 0x0080, AddRule = 0x0100, Style = 0x0200, TOC = 0x0400};
   QListViewItem* item;
   QString text;
   bool isEmpty();
   bool contains(const QString& sought, bool case_sensitive = false);
   bool change(const QString& s);
   void destroy();
   void updateView();
   State state();
   void open(QTextStream& ts);
   void save(QTextStream& ts, bool current = false);
   bool saveHTML(const KURL& fname, const QString& origname, const QString& style,
      int flags = SaveAll);
   void addLink(const QString& s);
   void addLink(const TNoteLink& l);
   int linkCount() const;
   const TNoteLink& link(int i) const;
   void removeLink(int i);
   void modifyLink(int i, const QString& s);
   void modifyLink(int i, TNoteLink& l);
   void modifyLinkDescription(int i, const QString& s);
   TNote(QListViewItem* i, TNotesCollection* coll);
   TNote(QListViewItem* i, TNotesCollection* coll, const QString& s);
   ~TNote();
};
#endif

