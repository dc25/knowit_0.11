/***************************************************************************
                          knowitpref.cpp  -  description
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

#include "knowitpref.h"


#include <qcheckbox.h>
#include <qcolor.h>
#include <qtoolbutton.h>
#include <qspinbox.h>
#include <qdatetime.h>
#include <qlayout.h>
#include <qwhatsthis.h>
#include <qstring.h>
#include <qlabel.h>
#include <qvbox.h>

#include <kconfig.h>
#include <kcombobox.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kdeversion.h>
#include <kfontdialog.h>
#include <klineedit.h>
#include <kcolorbutton.h>
#include <kdualcolorbutton.h>

void KnowitOptions::save(KConfig* config) const
{
   config->setGroup("KDE");
   config->writeEntry("MultipleInstances", multipleInstances);
   config->setGroup("General");
   config->writeEntry("Docked", docked);
   config->writeEntry("Autosave", autosave);
   config->writeEntry("Reopen", reopen);
   config->writeEntry("HorizontalSplit", horizontalSplit);
   config->writeEntry("UnconditionalSave", unconditionalSave);
   config->writeEntry("Backup", backup);
   config->writeEntry("DefaultName", defaultName);
   config->writeEntry("AlternateTree", alternateTree);
   config->writeEntry("AlternateColor", alternateColor);
   config->writeEntry("AutoCollapse", autoCollapse);

   config->setGroup("Links");
   config->writeEntry("First", firstLink);
   config->writeEntry("Format", linkFormat);

   config->setGroup("Editor");
   config->writeEntry("WordWrap", wordwrap);
   config->writeEntry("EnterBreakLine", enterBreakLine);
   config->writeEntry("TabFocus", tabfocus);
   config->writeEntry("CustomColors", customColors);
   config->writeEntry("ForeEditColor", foreEditColor);
   config->writeEntry("BackEditColor", backEditColor);
   config->writeEntry("CustomFont", customFont);
   config->writeEntry("FontFamily", fontFamily);
   config->writeEntry("FontSize", fontSize);

   config->setGroup("Templates");
   config->writeEntry("InsertDateFormatString", insertDateFormat);
   config->writeEntry("InsertDateColor", insertDateColor);
   config->writeEntry("InsertDateBold", insertDateBold);
   config->writeEntry("InsertDateItalic", insertDateItalic);
   config->writeEntry("InsertDateUnderline", insertDateUnderline);

   config->setGroup("Export");
   config->writeEntry("Flags", exportFlags);

}


void KnowitOptions::read(KConfig* config)
{
   config->setGroup("KDE");
   multipleInstances = config->readBoolEntry("MultipleInstances", true);
   config->setGroup("General");
   docked = config->readBoolEntry("Docked", false);
   autosave = config->readNumEntry("Autosave", 0);
   reopen = config->readBoolEntry("Reopen", true);
   horizontalSplit = config->readNumEntry("HorizontalSplit", true);
   defaultName = config->readEntry("DefaultName", i18n("New note"));
   alternateTree = config->readBoolEntry("AlternateTree", false);
   alternateColor = config->readEntry("AlternateColor", QString("LightYellow"));
   autoCollapse = config->readBoolEntry("AutoCollapse", false);
   unconditionalSave = config->readBoolEntry("UnconditionalSave", false);
   backup = config->readBoolEntry("Backup", true);
   
   config->setGroup("Links");
   firstLink = config->readBoolEntry("First", true);
   linkFormat = config->readNumEntry("Format", 0);

   config->setGroup("Editor");
   wordwrap = config->readBoolEntry("WordWrap", true);
   enterBreakLine = config->readBoolEntry("EnterBreakLine", true);
   tabfocus = config->readBoolEntry("TabFocus", false);
   customColors = config->readBoolEntry("CustomColors", false);
   foreEditColor = config->readEntry("ForeEditColor", QString("Black"));
   backEditColor = config->readEntry("BackEditColor", QString("White"));
   customFont = config->readBoolEntry("CustomFont", false);
   fontFamily = config->readEntry("FontFamily", QString("Helvetica"));
   fontSize = config->readNumEntry("FontSize", 10); // KDE-Default?

   config->setGroup("Templates");
   insertDateFormat = config->readEntry("InsertDateFormatString", QString("ddd d MMM yyyy, hh:mm:ss"));
   insertDateColor = config->readEntry("InsertDateColor", QString("Blue"));
   insertDateBold = config->readBoolEntry("InsertDateBold", false);
   insertDateItalic = config->readBoolEntry("InsertDateItalic", true);
   insertDateUnderline = config->readBoolEntry("InsertDateUnderline", false);

   config->setGroup("Export");
   exportFlags = config->readNumEntry("Flags", 0);
}



KnowitPreferences::KnowitPreferences()
   : KDialogBase(IconList, i18n("KnowIt Preferences"), Ok|Cancel, Ok)
{
    QFrame* page;
    QVBoxLayout* layout;

    /* first page: General */
    page = addPage(i18n("General"), i18n("General options"),
       KGlobal::iconLoader()->loadIcon(QString("configure"), KIcon::Toolbar, KIcon::SizeMedium));
    layout = new QVBoxLayout(page, 0, spacingHint());
    layout->addWidget(docked = new QCheckBox(i18n("&Dock in System tray"), page, "Dock"));
    layout->addWidget(reopen = new QCheckBox(i18n("Open &last file on startup"), page, "Reopen"));

    QHBox* autosaveBox = new QHBox(page);
    layout->addWidget(autosaveBox);
    new QLabel(i18n("Autosave:"), autosaveBox);
    autosave = new QSpinBox(0, 120, 5, autosaveBox, "Autosave");
    autosave->setPrefix(i18n("every "));
    autosave->setSuffix(i18n(" min."));
    autosave->setSpecialValueText(i18n("Never"));
    QWhatsThis::add(autosave, i18n("Current file will be automatically saved "
       "after the specified interval. Set to <i>never</i> to disable autosave."));

    layout->addWidget(unconditionalSave = new QCheckBox(i18n("Automatically save file on e&xit"),
        page, "UnconditionalSave"));
    QWhatsThis::add(unconditionalSave, i18n("Current file will be automatically saved "
       "on exit without confirmation."));
    layout->addWidget(backup = new QCheckBox(i18n("Create &backups"),
        page, "Backup"));
    QWhatsThis::add(backup, i18n("Create backup of current document before it is saved."));
    layout->addWidget(multipleInstances = new QCheckBox(i18n("Allow &multiple instances of KnowIt"),
        page, "Instances"));
    QWhatsThis::add(multipleInstances, i18n("If this option is disabled, only one "
       "instance of KnowIt will be allowed. If there is another instance already running, "
       "it will be automatically activated instead of running new one."));
    layout->addStretch(1);


    /* second page: Interface */
    page = addPage(i18n("Interface"), i18n("Interface options"),
       KGlobal::iconLoader()->loadIcon(QString("misc"), KIcon::Toolbar, KIcon::SizeMedium));
    layout = new QVBoxLayout(page, 0, spacingHint());
    layout->addWidget(horizontalSplit = new QCheckBox(i18n("Split window &horizontally"), page, "Split"));
    QWhatsThis::add(horizontalSplit, i18n("If this option is set, notes tree will "
       "be displayed on the left and the editor on the right (this is default).<br>"
       "Otherwise notes tree will be displayed at the top and the editor at the bottom."));

    QHBox* defaultNameBox = new QHBox(page);
    layout->addWidget(defaultNameBox);
    new QLabel(i18n("Default note name:"), defaultNameBox);
    defaultName = new KLineEdit(defaultNameBox, "DefaultName");
    QWhatsThis::add(defaultName, i18n("Default name for new notes. It would be "
       "autoselected, so if you often copy text using mouse selection, you may "
       "consider setting default name to none."));

    QHBox* linkBox = new QHBox(page);
    layout->addWidget(linkBox);
    new QLabel(i18n("Link format:"), linkBox);
    linkFormat = new KComboBox(linkBox, "LinkFormat");
    linkFormat->insertItem(i18n("Description (link)"));
    linkFormat->insertItem(i18n("Link (description)"));
    linkFormat->insertItem(i18n("Link only"));
    linkFormat->insertItem(i18n("Description only"));
   
    QHBox* alternateBox = new QHBox(page);
    layout->addWidget(alternateBox);
    alternateTree = new QCheckBox(i18n("Alternate colors in tree"), alternateBox, "Alternate");
    alternateColor = new KColorButton(QColor("White"), alternateBox, "AlternateColor");

    layout->addWidget(autoCollapse = new QCheckBox(i18n("Automatically collapse other notes"), page, "AutoCollapse"));
    QWhatsThis::add(autoCollapse, i18n("If this option is set, only current subtree "
       "will be visible, other notes will be automatically collapsed."));
    layout->addStretch(1);


    /* third page: Editor */
    page = addPage(i18n("Editor"), i18n("Editor options"),
       KGlobal::iconLoader()->loadIcon(QString("edit"), KIcon::Toolbar, KIcon::SizeMedium));
    layout = new QVBoxLayout(page, 0, spacingHint());
    layout->addWidget(wordwrap = new QCheckBox(i18n("Use &word wrap"),
       page, "WordWrap"));
    layout->addWidget(enterBreakLine = new QCheckBox(i18n("'Enter' ends current line, not paragraph"),
       page, "EnterLineBreak"));
    layout->addWidget(tabfocus = new QCheckBox(i18n("'Tab' in editor changes focus"),
       page, "TabFocus"));
#if KDE_VERSION_MAJOR == 3 && KDE_VERSION_MINOR < 1
    tabfocus->hide();
#endif
    QHBox* colorBox = new QHBox(page);
    layout->addWidget(colorBox);
    customColors = new QCheckBox(i18n("Use &custom colors"), colorBox, "CustomColors");
    editColors = new KDualColorButton(colorBox);

    const char fontSizes[] = {4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,22,24,26,28,32,48,64,0};
    QStringList fontList;
    KFontChooser::getFontList(fontList, false);

    QHBox* fontBox = new QHBox(page);
    layout->addWidget(fontBox);
    customFont = new QCheckBox(i18n("Use custom font:"), fontBox, "customFont");
    fontFamily = new KComboBox(true, fontBox);
    fontFamily->insertStringList(fontList);
    fontSize = new KComboBox(true, fontBox);
    for( int i=0; fontSizes[i] != 0; i++ ){
       fontSize->insertItem(QString().setNum(fontSizes[i]),i);
    }

    layout->addStretch(1);


    /* fourth page: Templates */
    page = addPage(i18n("Templates"), i18n("Templates configuration"),
       KGlobal::iconLoader()->loadIcon(QString("wizard"), KIcon::Toolbar, KIcon::SizeMedium));
    layout = new QVBoxLayout(page, 0, spacingHint());
    QWhatsThis::add(page, i18n("<html>\n"
"<p>These expressions may be used: </p>\n"
"<table>\n"
"<tr><th>d</th><td>the day as number without a leading zero (1-31)</td></tr>\n"
"<tr><th>dd</th><td>the day as number with a leading zero (01-31)</td></tr>\n"
"<tr><th>ddd</th><td>the abbreviated localized day name (e.g. 'Mon'..'Sun')</td></tr>\n"
"<tr><th>dddd</th><td>the long localized day name (e.g. 'Monday'..'Sunday')</td></tr>\n"
"<tr><th>M</th><td>the month as number without a leading zero (1-12)</td></tr>\n"
"<tr><th>MM</th><td>the month as number with a leading zero (01-12)</td></tr>\n"
"<tr><th>MMM</th><td>the abbreviated localized month name (e.g. 'Jan'..'Dec')</td></tr>\n"
"<tr><th>MMMM</th><td>the long localized month name (e.g. 'January'..'December')</td></tr>\n"
"<tr><th>yy</th><td>the year as two digit number (00-99)</td></tr>\n"
"<tr><th>yyyy</th><td>the year as four digit number (1752-8000)</td></tr>\n"
"<tr><th>h</th><td>the hour without a leading zero (0..23 or 1..12 if AM/PM display)</td></tr>\n"
"<tr><th>hh</th><td>the hour with a leading zero (00..23 or 01..12 if AM/PM display)</td></tr>\n"
"<tr><th>m</th><td>the minute without a leading zero (0..59)</td></tr>\n"
"<tr><th>mm</th><td>the minute with a leading zero (00..59)</td></tr>\n"
"<tr><th>s</th><td>the second without a leading zero (0..59)</td></tr>\n"
"<tr><th>ss</th><td>the second with a leading zero (00..59)</td></tr>\n"
"<tr><th>AP</th><td>use AM/PM display</td></tr>\n"
"<tr><th>AP</th><td>use am/pm display</td></tr>\n"
"</table></html>"));

    QHBox* topBox = new QHBox(page);
    layout->addWidget(topBox);
    QLabel * label1 = new QLabel(i18n("Date Format: "), topBox);
    label1->setFixedSize(label1->sizeHint());
    insertDatePreview = new QLabel("", topBox);
    
    QHBox *tmpBox = new QHBox(page);
    layout->addWidget(tmpBox);
    insertDateColorButton = new KColorButton(QColor("Blue"), tmpBox, "InsertDateColorButton");
    insertDateFormatEdit = new KLineEdit(tmpBox, "InsertDateFormatEdit");
    insertDateItalicButton = new QToolButton(tmpBox,"InsertDateItalicButton");
    insertDateItalicButton->setIconSet(QIconSet(KGlobal::iconLoader()->loadIcon("text_italic", KIcon::Toolbar)));
    insertDateItalicButton->setToggleButton(true);
    insertDateBoldButton = new QToolButton(tmpBox,"InsertDateBoldButton");
    insertDateBoldButton->setIconSet(QIconSet(KGlobal::iconLoader()->loadIcon("text_bold", KIcon::Toolbar)));
    insertDateBoldButton->setToggleButton(true);
    insertDateUnderlineButton = new QToolButton(tmpBox, "InsertDateUnderlineButton");
    insertDateUnderlineButton->setIconSet(QIconSet(KGlobal::iconLoader()->loadIcon("text_under", KIcon::Toolbar)));    
    insertDateUnderlineButton->setToggleButton(true);
    insertDateColorButton->setFixedSize(insertDateBoldButton->sizeHint());
    insertDateFormatEdit->setMinimumWidth(6*insertDateBoldButton->sizeHint().width());
    layout->addStretch(1);

    setIconListAllVisible(true);
}

void KnowitPreferences::setOptions(const KnowitOptions& Options)
{
   docked->setChecked(Options.docked);
   reopen->setChecked(Options.reopen);
   autosave->setValue(Options.autosave);
   unconditionalSave->setChecked(Options.unconditionalSave);
   backup->setChecked(Options.backup);
   multipleInstances->setChecked(Options.multipleInstances);

   horizontalSplit->setChecked(Options.horizontalSplit);
   defaultName->setText(Options.defaultName);
   alternateTree->setChecked(Options.alternateTree);
   alternateColor->setColor(QColor(Options.alternateColor));
   autoCollapse->setChecked(Options.autoCollapse);
   linkFormat->setCurrentItem(Options.linkFormat);
         
   wordwrap->setChecked(Options.wordwrap);
   enterBreakLine->setChecked(Options.enterBreakLine);   
   tabfocus->setChecked(Options.tabfocus);
   customColors->setChecked(Options.customColors);
   editColors->setForeground(QColor(Options.foreEditColor));
   editColors->setBackground(QColor(Options.backEditColor));
   customFont->setChecked(Options.customFont);
   fontFamily->setCurrentText(Options.fontFamily);
   fontSize->setCurrentText(QString().setNum(Options.fontSize));
   
   insertDateFormatEdit->setText(Options.insertDateFormat);
   insertDateColorButton->setColor(QColor(Options.insertDateColor));
   insertDateBoldButton->setOn(Options.insertDateBold);
   insertDateItalicButton->setOn(Options.insertDateItalic);
   insertDateUnderlineButton->setOn(Options.insertDateUnderline);
   slotUpdateDatePreview();
   connect(insertDateColorButton, SIGNAL(clicked()), this, SLOT(slotUpdateDatePreview()));
   connect(insertDateFormatEdit, SIGNAL(textChanged (const QString &)), this, SLOT(slotUpdateDatePreview()));
   connect(insertDateItalicButton, SIGNAL(toggled(bool)), this, SLOT(slotUpdateDatePreview()));
   connect(insertDateBoldButton, SIGNAL(toggled(bool)), this, SLOT(slotUpdateDatePreview()));
   connect(insertDateUnderlineButton, SIGNAL(toggled(bool)), this, SLOT(slotUpdateDatePreview()));

   /* non-GUI options */
   firstLink = Options.firstLink;
   exportFlags = Options.exportFlags;
}

KnowitOptions KnowitPreferences::getOptions() const
{
   KnowitOptions Options;
   Options.docked = docked->isChecked();
   Options.reopen = reopen->isChecked();
   Options.autosave = autosave->value();
   Options.unconditionalSave = unconditionalSave->isChecked();   
   Options.multipleInstances = multipleInstances->isChecked();
   Options.backup = backup->isChecked();
   
   Options.horizontalSplit = horizontalSplit->isChecked();
   Options.defaultName = defaultName->text();
   Options.alternateTree = alternateTree->isChecked();
   Options.alternateColor = alternateColor->color().name();
   Options.autoCollapse = autoCollapse->isChecked();
   Options.linkFormat = linkFormat->currentItem();
   
   Options.wordwrap = wordwrap->isChecked();
   Options.enterBreakLine = enterBreakLine->isChecked();   
   Options.tabfocus = tabfocus->isChecked();
   Options.customColors = customColors->isChecked();
   Options.foreEditColor = editColors->foreground().name();
   Options.backEditColor = editColors->background().name();
   Options.customFont = customFont->isChecked();
   Options.fontFamily = fontFamily->currentText();
   Options.fontSize = fontSize->currentText().toInt();

   Options.insertDateFormat = insertDateFormatEdit->text();
   Options.insertDateColor = insertDateColorButton->color().name();
   Options.insertDateBold = insertDateBoldButton->isOn();
   Options.insertDateItalic = insertDateItalicButton->isOn();
   Options.insertDateUnderline = insertDateUnderlineButton->isOn();

   Options.firstLink = firstLink;
   Options.exportFlags = exportFlags;
   return Options;
}

void KnowitPreferences::slotUpdateDatePreview()
{
   QString str = "<html><p align=\"center\">";
   if(insertDateBoldButton->isOn()) str += "<b>";
   if(insertDateItalicButton->isOn()) str +="<i>";
   if(insertDateUnderlineButton->isOn()) str += "<u>";
   str += "<font COLOR=\""+insertDateColorButton->color().name()+"\">";
   str += QString("%1").arg(QDateTime::currentDateTime().toString(insertDateFormatEdit->text()));
   str += "</font>";
   if(insertDateUnderlineButton->isOn()) str += "</u>";
   if(insertDateItalicButton->isOn()) str +="</i>";
   if(insertDateBoldButton->isOn()) str += "</b>";
   str += "</p></html>";
   insertDatePreview->setPaletteBackgroundColor(QColor("White"));
   insertDatePreview->setText(str);
}


