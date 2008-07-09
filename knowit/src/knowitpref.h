/***************************************************************************
                          knowitpref.h  -  description
                             -------------------
    begin                : czw lis 28 2002
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

#ifndef KNOWITPREF_H
#define KNOWITPREF_H

#include <qstring.h>
#include <kdialogbase.h>

class QCheckBox;
class QSpinBox;
class QToolButton;
class KLineEdit;
class KColorButton;
class KDualColorButton;
class KConfig;
class KComboBox;

class KnowitOptions
{
public:
   /* General */
   bool docked;
   bool reopen;
   int autosave;
   bool unconditionalSave;
   bool multipleInstances;
   bool backup;
   /* GUI */
   bool horizontalSplit;
   int linkFormat;
   QString defaultName;
   bool alternateTree;
   QString alternateColor;
   bool autoCollapse;
   /* Links */
   bool firstLink;
   /* Editor */
   bool wordwrap;
   bool enterBreakLine;
   bool tabfocus;
   bool customColors;
   QString backEditColor;
   QString foreEditColor;
   bool customFont;
   QString fontFamily;
   int fontSize;
   /* Date format */
   QString insertDateFormat, insertDateColor;
   bool insertDateBold, insertDateItalic, insertDateUnderline;
   void save(KConfig* config) const;
   void read(KConfig* config);
   /* Export */
   int exportFlags;
};


class KnowitPreferences : public KDialogBase
{
Q_OBJECT
private:
   QCheckBox *docked, *reopen, *horizontalSplit,  *multipleInstances, *backup;
   QCheckBox *unconditionalSave;
   QCheckBox *wordwrap, *tabfocus, *customColors;
   KDualColorButton* editColors;
   QCheckBox *enterBreakLine;   
   KLineEdit *defaultName;
   QCheckBox *alternateTree;
   KColorButton *alternateColor;
   QCheckBox* autoCollapse;
   QCheckBox* customFont;
   KComboBox *fontFamily, *fontSize;
   KComboBox *linkFormat;
   KLineEdit *insertDateFormatEdit;
   KColorButton *insertDateColorButton;
   QLabel *insertDatePreview;
   QSpinBox* autosave;
   QToolButton *insertDateItalicButton, *insertDateBoldButton, *insertDateUnderlineButton;
   bool firstLink;
   int exportFlags;   
public:
    KnowitPreferences();
    void setOptions(const KnowitOptions& Options);
    KnowitOptions getOptions() const;
    public slots:
    void slotUpdateDatePreview();
};

#endif

