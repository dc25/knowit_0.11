/***************************************************************************
                          knowitchooser.cpp  -  description
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

#include "knowitchooser.h"

#include <qcheckbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qvbox.h>

#include <kcombobox.h>
#include <klocale.h>


KnowitChooser::KnowitChooser(int flags) 
   : KDialogBase(Plain, i18n("Export"), Ok|Cancel, Ok)
{
   QLabel* l1;
   QLabel* l2;
   QVBoxLayout* layout = new QVBoxLayout(plainPage());
   layout->addWidget(l1 = new QLabel(i18n("&Exported notes:"), plainPage()));
   layout->addWidget(notes = new KComboBox(plainPage(), "Notes"));
   l1->setBuddy(notes);
   notes->insertItem(i18n("All"));
   notes->insertItem(i18n("Current with children"));
   notes->insertItem(i18n("Current only"));
    
   layout->addWidget(l2 = new QLabel(i18n("&Encoding:"), plainPage()));
   layout->addWidget(charset = new KComboBox(plainPage(), "Encoding"));
   l2->setBuddy(charset);
   charset->insertItem(i18n("Local (8-bit)"));
   charset->insertItem(i18n("UTF-8"));

   layout->addItem(new QSpacerItem(0, 10));
   layout->addWidget(enumerate = new QCheckBox(i18n("Numbered titles"),
      plainPage(), "Numbered"));
   layout->addWidget(toc = new QCheckBox(i18n("Table of contents"),
      plainPage(), "TOC"));
   layout->addWidget(rule = new QCheckBox(i18n("Rule betweeen notes"),
      plainPage(), "Rule"));
   layout->addWidget(style = new QCheckBox(i18n("Use editor font and colors"),
      plainPage(), "Style"));
   setChoice(flags);
}

KnowitChooser::~KnowitChooser()
{
}

int KnowitChooser::getChoice() const
{
   int flags = 2 - notes->currentItem();
   if (charset->currentItem() == 1) flags = flags | UseUTF8;
   if (enumerate->isChecked()) flags = flags | Enumerate;
   if (toc->isChecked()) flags = flags | TOC;
   if (rule->isChecked()) flags = flags | AddRule;
   if (style->isChecked()) flags = flags | Style;
   return flags;
}

void KnowitChooser::setChoice(int flags)
{
   if (flags & SaveAll) notes->setCurrentItem(0);
   else if (flags & SaveSubnotes) notes->setCurrentItem(1);
   else notes->setCurrentItem(2);
   charset->setCurrentItem((flags & UseUTF8) != 0);
   enumerate->setChecked(flags & Enumerate);
   toc->setChecked(flags & TOC);
   rule->setChecked(flags & AddRule);
   style->setChecked(flags & Style);
}
