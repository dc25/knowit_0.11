/***************************************************************************
                          notes.cpp  -  description
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

#include "notes.h"
#include <qstringlist.h>
#include <qfile.h>
#include <qdatetime.h>
#include <qtextstream.h>
#include <qtextcodec.h>
#include <klocale.h>
#include <klistview.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kmimetype.h>
#include <krun.h>
#include <kurl.h>
#include <klistview.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#else
#define VERSION "?"
#endif


void TNoteLink::save(QTextStream& ts) const
{
   if (!link.isEmpty()) {
      ts << "\\Link " << link << "\n";
      if (!description.isEmpty())
         ts << "\\Descr " << description << "\n";
      }
}

QString TNoteLink::text(int fmt) const
{
   if (fmt == LinkOnly || description.isEmpty()) return link;
   else if (fmt == DescriptionOnly) return description;
   else if (fmt == LinkDescription)
      return QString("%1 (%2)").arg(link).arg(description);
   else return QString("%1 (%2)").arg(description).arg(link);
}

QPixmap TNoteLink::icon() const
{
   return KGlobal::iconLoader()->loadIcon(KMimeType::iconForURL(KURL(link)),
      KIcon::Small);
}

void TNoteLink::open() const
{
   if (!isLocalReference())
      new KRun(KURL(link), 0, false, true);
}

void TNoteLink::openWith() const
{
   if (!isLocalReference())
      KRun::displayOpenWithDialog(KURL(link));
}

bool TNoteLink::isLocalReference() const
{
   KURL url(link);
   return url.protocol() == "knowit";
}





TNotesCollection::TNotesCollection()
{
   Pixmaps[0] = KGlobal::iconLoader()->loadIcon("ascii", KIcon::Small);
   Pixmaps[1] = KGlobal::iconLoader()->loadIcon("txt", KIcon::Small);
   Pixmaps[2] = KGlobal::iconLoader()->loadIcon("folder", KIcon::Small);
   Pixmaps[3] = KGlobal::iconLoader()->loadIcon("folder_txt", KIcon::Small);
   modified = false;
   autoUpdate = true;
}



TNotesCollection::~TNotesCollection()
{}

void TNotesCollection::addNote(QListViewItem* item)
{
   insert(item, new TNote(item, this));
   QListViewItem* parent = item->parent();
   if (parent) find(parent)->updateView();
   modified = true;
}

void TNotesCollection::addNote(QListViewItem* item, const QString& s)
{
   insert(item, new TNote(item, this, s));
   QListViewItem* parent = item->parent();
   if (parent) find(parent)->updateView();
   modified = true;
}

void TNotesCollection::removeNote(QListViewItem* item)
{
   TNote* note = find(item);
   if (!note) qWarning("Internal error while removing note\n");
   else {
      QListViewItem* parent = item->parent();
      for (QListViewItemIterator it(item); it.current() &&
        (it.current() == item || it.current()->depth() > item->depth()); ++it)
         find(it.current())->destroy();
      delete item;
      if (parent) find(parent)->updateView();
      modified = true;
   }
}

bool TNotesCollection::changeNote(QListViewItem* item, const QString& s)
{
   TNote* changed = find(item);
   if (!changed) {
      qWarning("Internal error while changing note\n");
      return false;
   }
   if (changed->change(s))
      return modified = true;
   else
      return false;
}

void TNotesCollection::clearNotes()
{
   setAutoDelete(true);
   clear();
   setAutoDelete(false);
}

const QString& TNotesCollection::text(QListViewItem* item)
{
   TNote* note = find(item);
   if (!note)
      qFatal("Internal error while accessing note text\n");
   return note->text;
}

void TNotesCollection::updateNotesView()
{
   for (QPtrDictIterator<TNote> it(*this); it.current(); ++it)
      it.current()->updateView();
}

int TNotesCollection::wordCount(const QString& s)
{
  int words = 0;
  bool inTag = false;
  for (uint i = 0; i < s.length(); i++)
  {
    if (inTag && s[i] == '>')
      inTag = false;
    else if (!inTag && s[i] == '<')
      inTag = true;
   else if (!inTag && s[i].isLetter() && (!i || !s[i-1].isLetter()))
      words++;
  }
  return words;
}

int TNotesCollection::charCount(const QString& s)
{
  int chars = 0;
  bool inTag = false;
  for (uint i = 0; i < s.length(); i++)
  {
    if (inTag && s[i] == '>')
      inTag = false;
    else if (!inTag && s[i] == '<')
      inTag = true;
    else if (!inTag && s[i] != '\n')
      chars++;
    else if (!inTag && s[i] == '\n' && i && s[i-1] != '>' && s[i+1] != '<')
      chars++;
  }
  return chars;
}

int TNotesCollection::parsCount(const QString& text)
{
  return text.contains("<p ") + text.contains("<p>");
}


TNote::TNote(QListViewItem* i, TNotesCollection* coll) : text()
{
   collection = coll;
   item = i;
   updateView();
}

TNote::TNote(QListViewItem* i, TNotesCollection* coll, const QString& s) :
   text(s)
{
   collection = coll;
   item = i;
   updateView();
}

bool TNote::isEmpty()
{
   return !text.length();
}

bool TNote::contains(const QString& sought, bool case_sensitive)
{
   return text.contains(sought, case_sensitive);
}


TNote::~TNote()
{
}

void TNote::destroy()
{
   collection->remove(item);
   delete this;
}

bool TNote::change(const QString& s)
{
   if (text == s)
      return false;
   text = s;
   updateView();
   return true;
}

TNote::State TNote::state()
{
   if (text.stripWhiteSpace().isEmpty())
      return (item->childCount()) ? EmptyParent : Empty;
   else
      return (item->childCount()) ? TextParent : Text;
}

void TNote::updateView()
{
   if (collection->autoUpdate)
      item->setPixmap(0, collection->Pixmaps[state()]);
}

bool TNote::saveHTML(const KURL& fname, const QString& origname, const QString& style,
   int flags)
{
  QFile file(fname.path());
  if (!file.open( IO_WriteOnly))
     return false;
  QTextStream ts(&file);
  if (flags & UseUTF8) ts.setEncoding(QTextStream::UnicodeUTF8);
  else ts.setEncoding(QTextStream::Locale);
  ts << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n"
     "<html>\n<head>\n   <title>" << i18n("KnowIt document") <<
     QString("</title>\n   <meta http-equiv=\"Content-Type\" "
     "content=\"text/html; charset=%1\">\n").arg(ts.codec()->mimeName());
  if (!style.isEmpty())
     ts << "   <style type=\"text/css\">\n   " << style << "\n   </style>\n";
  ts << "</head>\n\n<body>\n";

  QValueVector<int> Depths;
  Depths.append(0);
  if (flags & TOC) {
     ts << QString("<h1>%1</h1>\n").arg(i18n("Table of contents"));
     saveHTMLTocEntry(ts, Depths, flags);
     ts << "\n\n";
     }

  Depths.clear();
  Depths.append(0);
  saveHTMLBuf(ts, Depths, flags);
  
  QString epilog = i18n("Exported from %1 by KnowIt %2, %3.");
  ts << "<br><br><br><small>" << epilog.arg(origname)
     .arg(VERSION)
     .arg(QDateTime::currentDateTime().toString()) << "</small>\n\n";
  ts << "</body>\n</html>\n\n";
  return true;
}

bool TNote::saveHTMLBuf(QTextStream& ts, QValueVector<int>& depths,
  int flags)
{
   int hlevel = depths.count();
   depths.last() = depths.last()+1;
   if (hlevel < 1) hlevel = 1;
   else if (hlevel > 3) hlevel = 3;
   QString id, number;
   for (uint i=0; i<depths.count(); i++)
         id += QString("%1.").arg(depths[i]);
   if (flags & Enumerate)
      number = id + " ";
   
   ts << QString("<h%1 id=\"S%2\">%3%4</h%5>").arg(hlevel).arg(id).arg(number)
      .arg(item->text(0)).arg(hlevel);
   QString htmltext = text;
   int begin = htmltext.find("<body");
   if (begin >= 0) begin = htmltext.find(">", begin);
   int end = htmltext.find("</body>");
   if (begin < 0) begin = 0;
   if (end <= begin) end = htmltext.length();
   ts << htmltext.mid(begin+1, end-begin-1);

   /* save links */
   for (QValueList<TNoteLink>::Iterator LinkList = links.begin();
      LinkList != links.end(); ++LinkList)
      ts << QString("<a href=\"%1\">%2</a><br>\n").arg((*LinkList).link)
         .arg((*LinkList).text(TNoteLink::DescriptionOnly));

   /* save rule */
   if (flags & AddRule) ts << "<hr size=\"1\" noshade>\n\n";
   else ts << "\n\n";

   /* save children */
   if ((SaveSubnotes | SaveAll) & flags && item->childCount()) {
      depths.append(0);
      collection->find(item->firstChild())->saveHTMLBuf(ts, depths, flags);
      }
   if ((SaveSubnotes | SaveAll) & flags && item->nextSibling())
      collection->find(item->nextSibling())->saveHTMLBuf(ts, depths, flags);

   if (!item->nextSibling()) depths.pop_back();
   return true;
}

bool TNote::saveHTMLTocEntry(QTextStream& ts, QValueVector<int>& depths, int flags)
{
   QString space;
   space.fill(' ', depths.count());
   depths.last() = depths.last()+1;
   if (depths.last() == 1) {
      ts << space;
      if (depths.count() != 1) ts << "<dd>";
      ts << "<dl>\n";
      }

   QString id, number;
   for (uint i=0; i<depths.count(); i++)
         id += QString("%1.").arg(depths[i]);
   if (flags & Enumerate) number = id + " ";

   ts << space;
   ts << QString("<dt><a href=\"#S%1\">%2%3</a></dt>\n").arg(id).arg(number).
      arg(item->text(0));
   if ((SaveSubnotes | SaveAll) & flags && item->childCount()) {
      depths.append(0);
      collection->find(item->firstChild())->saveHTMLTocEntry(ts, depths, flags);
      }
   if ((SaveSubnotes | SaveAll) & flags && item->nextSibling())
      collection->find(item->nextSibling())->saveHTMLTocEntry(ts, depths, flags);
   if (!item->nextSibling()) {
      depths.pop_back();
      ts << space;
      ts << "</dl>";
      if (depths.count()) ts << "</dd>";
      ts << "\n";
      }
   return true;
}

void TNote::addLink(const QString& s)
{
   links.append(TNoteLink(s));
   collection->modified = true;
}

void TNote::addLink(const TNoteLink& l)
{
   links.append(l);
   collection->modified = true;
}

void TNote::removeLink(int i)
{
   links.remove(links[i]);
   collection->modified = true;
}

void TNote::modifyLink(int i, const QString& s)
{
   links[i].link = s;
   collection->modified = true;
}

void TNote::modifyLink(int i, TNoteLink& l)
{
   if (l != links[i])
      collection->modified = true;
   links[i] = l;
}

void TNote::modifyLinkDescription(int i, const QString& s)
{
   links[i].description = s;
   collection->modified = true;
}

int TNote::linkCount() const
{
   return links.count();
}

const TNoteLink& TNote::link(int i) const
{
   return links[i];
}


void TNote::save(QTextStream& ts, bool current)
{
   const QString header = "\\NewEntry %1 %2\n";
   const QString activeheader = "\\CurrentEntry %1 %2\n";

   if (current)
      ts << activeheader.arg(item->depth()).arg(item->text(0));
   else
      ts << header.arg(item->depth()).arg(item->text(0));
   for (QValueList<TNoteLink>::Iterator LinkList = links.begin();
        LinkList != links.end(); ++LinkList)
        (*LinkList).save(ts);
   ts << text;
   if (!text.isEmpty())
      ts << '\n';
   ts << '\n';
}

void TNote::open(QTextStream& ts)
{
   QString s;
   text = "";
   links.clear();

   while (true) {
      s = ts.readLine();
      if (s.isNull() || s.isEmpty())
         break;
      if (s.left(6) == "\\Link ")
         addLink(s.mid(6));
      else if (s.left(7) == "\\Descr " && links.count()) 
         modifyLinkDescription(links.count()-1, s.mid(7));
      else {
         if (text.length())
            text += '\n';
         text += s;
      }
   }
}
