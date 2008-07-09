/***************************************************************************
                          knowitlink.cpp  -  description
                             -------------------
    begin                : pi± cze 27 2003
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

#include "knowitlink.h"
#include "notes.h"

#include <qtoolbutton.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qvbox.h>

#include <kfiledialog.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kcombobox.h>
#include <klineedit.h>

KnowitLinkDialog::KnowitLinkDialog()
   : KDialogBase(Plain, i18n("Modify link"), Ok|Cancel, Ok)
{
   QVBoxLayout* layout = new QVBoxLayout(plainPage());

   QHBox* typeBox = new QHBox(plainPage());
   layout->addWidget(typeBox);
   new QLabel(i18n("Referenced item:"), typeBox);
   linkType = new KComboBox(typeBox, "LinkType");
   linkType->insertItem(i18n("File or URL"));
   linkType->insertItem(i18n("KnowIt note"));

   layout->addItem(new QSpacerItem(0, 5));       
   QHBox* editBox = new QHBox(plainPage());
   layout->addWidget(editBox);
   linkValue = new KLineEdit("Link text", editBox);
   linkValue->setMinimumWidth(300);
   browse = new QToolButton(editBox, "Browse");
   browse->setIconSet(QIconSet(KGlobal::iconLoader()->loadIcon("fileopen",
      KIcon::Toolbar, KIcon::SizeSmall)));
   
   layout->addItem(new QSpacerItem(0, 5));
   layout->addWidget(new QLabel(i18n("Link description:"), plainPage()));
   layout->addWidget(linkDescription = new KLineEdit("Link description", plainPage()));

   connect(browse, SIGNAL(clicked()), this, SLOT(slotBrowse()));
   connect(linkType, SIGNAL(activated(int)), this, SLOT(slotTypeChanged(int)));
}

KnowitLinkDialog::~KnowitLinkDialog()
{
}

bool KnowitLinkDialog::modifyLink(TNoteLink& link)
{
   linkDescription->setText(link.description);
   linkType->setCurrentItem(link.isLocalReference());
   if (link.isLocalReference())
      linkValue->setText(link.link.remove(0, 9));
   else
      linkValue->setText(link.link);
   slotTypeChanged(linkType->currentItem());
   if (exec() == QDialog::Accepted) {
      link.description = linkDescription->text();
      if (linkType->currentItem())
         link.link = "knowit://" + linkValue->text();
      else
         link.link = linkValue->text();
      return true;
   }
   else
      return false;
}

void KnowitLinkDialog::slotBrowse()
{
   KURL url=KFileDialog::getOpenURL(QString::null,
      i18n("*|All files"), this, i18n("Choose link..."));
   if (!url.isEmpty()) {
      linkValue->setText(url.url());
      linkType->setCurrentItem(0);
      }
}

void KnowitLinkDialog::slotTypeChanged(int index)
{
   browse->setEnabled(!index);
}

