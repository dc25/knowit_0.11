/***************************************************************************
                          knowitchooser.h  -  description
                             -------------------
    begin                : pi? lut 14 2003
    copyright            : (C) 2003 by Micha? Rudolf
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

#ifndef KNOWITCHOOSER_H
#define KNOWITCHOOSER_H

#include <kdialogbase.h>

class QCheckBox;
class KComboBox;

/**
  *@author Micha³ Rudolf
  */

class KnowitChooser : public KDialogBase  {
   Q_OBJECT
protected:
   KComboBox* notes;
   KComboBox* charset;
   QCheckBox* enumerate;
   QCheckBox* toc;
   QCheckBox* rule;
   QCheckBox* style;
public: 
   enum {SaveCurrent = 0, SaveSubnotes = 0x0001, SaveAll = 0x0002, UseUTF8 = 0x0040,
         Enumerate = 0x0080, AddRule = 0x0100, Style = 0x0200, TOC = 0x0400};
   KnowitChooser(int flags = 0);
   ~KnowitChooser();
   int getChoice() const;
   void setChoice(int flags);
};

#endif
