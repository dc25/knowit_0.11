/***************************************************************************
                          knowitlink.h  -  description
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

#ifndef __KNOWITLINK_H__
#define __KNOWITLINK_H__

#include <kdialogbase.h>

class KComboBox;
class KLineEdit;
class TNoteLink;
class QToolButton;

class KnowitLinkDialog : public KDialogBase  {
   Q_OBJECT
protected:
   KComboBox* linkType;
   KLineEdit* linkValue;
   KLineEdit* linkDescription;
   QToolButton* browse;
public:
   KnowitLinkDialog();
   ~KnowitLinkDialog();
   bool modifyLink(TNoteLink& link);
public slots:
   void slotBrowse();
   void slotTypeChanged(int index);
};


#endif
