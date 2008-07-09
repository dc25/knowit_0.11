/***************************************************************************
                          knowit.h  -  description
                             -------------------
    begin                : czw wrz 26 08:27:40 CEST 2002
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

#ifndef KNOWIT_H
#define KNOWIT_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qurloperator.h>

#include <keditcl.h>
#include <klocale.h>
#include <kmainwindow.h>
#include <kurl.h>
#include "knowitpref.h"
#include "knowitchooser.h"
#include "knowitedit.h"
#include "knowitlink.h"
#include "notes.h"

class QSplitter;
class QTimer;
class KAction;
class KConfig;
class KListBox;
class KToggleAction;
class KRecentFilesAction;

class KnowitTray;
class KnowitEdit;
class KnowitTree;


/** Knowit is the base class of the project */
class Knowit : public KMainWindow
{
  Q_OBJECT
  private:
    static const QString Untitled;
    KConfig *config;
    QSplitter* Layout;
    KnowitTree* Items;
    KListBox* Links;
    KnowitEdit* Edit;
    KURL filename;
    QPixmap pixmapFolder, pixmapFolderText, pixmapText, pixmapEmpty;
    KEdFind FindDlg;
    KnowitOptions Options;
    KnowitPreferences PrefDlg;
    KnowitChooser ChooserDlg;
    KnowitLinkDialog LinkDlg;
    KnowitTray* systray;
    /* data items */
    TNotesCollection Notes;
    bool runMinimized;
    QListViewItem* lastSought;
    QString soughtText;
    QTimer* AutosaveTimer;
    /* for saving backups */
    QUrlOperator URLOperator;
    /* false by default, set to true on queryClose() */
    bool shuttingDown;
    /* actions */
    KRecentFilesAction *actionRecent;
    KToggleAction *actionBold, *actionItalic, *actionUnderline,
       *actionEditAlignLeft, *actionEditAlignRight, *actionEditAlignCenter,
       *actionEditAlignJustify, *actionRawTextMode;
    KAction *actionCut, *actionCopy, *actionUndo, *actionRedo;

    /** Insert/Overwrite status */
    enum {StatusText = 1, StatusOvr = 2};
  public:
    /** Constructor of main window */
    Knowit(QWidget* parent=0, const char *name=0);
    /** Standard destructor */
    ~Knowit();
    /** Opens document */
    bool open(const KURL& fname);
    /** Saves document */
    bool save(const KURL& fname);
    /** Removes all data, creante new document */
    void reset();
    /** Checks if file was changed since last save, if so,
       user is prompted to save changes */
    virtual bool queryClose();
    /** Saves options and exits */
    virtual bool queryExit();
    /** Reads configuration from disk */
    void readOptions();
    /** Saves configuration to disk */
    void saveOptions();

    /** Finds and shows next occurence of currently sought text
       - or show message that it couldn't be found */
    void find(QListViewItem* start = 0);
    /** Returns true if notes or current text were modified */
    bool modified();
    /** Reimplemented from QWidget to prevent from closing when docked */
    virtual void closeEvent(QCloseEvent* e);
    /** Apply current configuration */
    void applyOptions(const KnowitOptions& O, bool store = true);
    /** Returns current note */
    TNote* currentNote() const;

 public slots:
    /** Status bar text has changed - displays it */
    void slotStatusMsg(const QString& text);
    /** Shows requested popup menu for notes */
    void slotContextMenu(KListView*, QListViewItem*, const QPoint&);
    /** Shows given note */
    void slotNoteChanged(QListViewItem* item);
    /** Add same-level note */
    void slotNoteAdd();
    /** Adds subnote */
    void slotNoteAddChild();
    /** Changes note's name */
    void slotNoteRename();
    /** Removes current note */
    void slotNoteRemove();
    /** Sort subnotes */
    void slotNoteSort();

    /** Move note up */
    void slotNoteMoveUp();
    /** Move note down */
    void slotNoteMoveDown();
    /** Move note at the beginning of current list */
    void slotNoteMoveBegin();
    /** Move note at the end of current list */
    void slotNoteMoveEnd();
    /** Move note level up */
    void slotNoteMoveLeft();
    /** Move note level down */
    void slotNoteMoveRight();
    /** Note statistics */
    void slotNoteStatistics();

    /** Current item has changed - updates associated text and shows next */
    void slotItemChanged(QListViewItem*);
    /** Resets to a new file */
    void slotFileNew();
    /** Prompts for a file to open and opens it */
    void slotFileOpen();
    /** Opens selected of recent files */
    void slotFileRecent(const KURL&);
    /** Saves file using previous name */
    void slotFileSave();
    /** Autosaves file using previous name */
    void slotFileAutosave();
    /** Prompts for filename and saves file */
    void slotFileSaveAs();
    /** Exports all notes to HTML */
    void slotFileExport();
    /** Prints current note */
    void slotFilePrint();
    /** Shows basic file statistics */
    void slotFileInfo();
    /** Quits KnowIt */
    void slotFileQuit();
    /** Turns bold in edit window on/off */
    void slotEditBold();
    /** Turns italic in edit window on/off */
    void slotEditItalic();
    /** Turns underlining in edit window on/off */
    void slotEditUnderline();
    /** Changes color of selected/current text */
    void slotEditColor();
    /** Changes vertical alignment to superscript */
    void slotEditSuperscript();
    /** Changes vertical alignment to subscript */
    void slotEditSubscript();
    /** Restores normal vertical alignment */
    void slotEditNormal();
    /** Prompts for text and shows note containing it */
    void slotEditFind();
    /** Shows next note with sought text */
    void slotEditFindNext();
    /** Shows note with given title */
    void slotEditGoTo();
    /** Aligns text to the left */
    void slotEditAlignLeft();
    /** Aligns text to the right */
    void slotEditAlignRight();
    /** Justifies text */
    void slotEditAlignJustify();
    /** Centers text */
    void slotEditAlignCenter();
    /** Change selected paragraphs to list. Warning: uses obsolete Qt function */
    void slotEditListBullet();
    void slotEditListNumber();
    void slotEditListUpper();
    void slotEditListLower();
    void slotEditListNone();
    /** Inserts current date */
    void slotEditInsertDate();
    /** Inserts file into editor */
    void slotEditInsertFile();
    /** Switches active window (notes/editor) */
    void slotEditSwitch();
    /** Edited text has changed - update bold/italic/... status */
    void slotEditChanged();
    /** Cursor position has changed - update bold/italic/... status */
    void slotEditCursorChanged(int para, int pos);
    /** Shows options dialog */
    void slotOptions();
    /** Shows shortcut configuration dialog */
    void slotOptionsKeys();
    /** Shows toolbar configuration dialog */
    void slotOptionsToolbar();
    /** Removes KnowIt from Applet Tray */
    void slotUndock();
    /** Shows tip of the day */
    void slotHelpTip();
    /** Updates available commands */
    void slotActionUpdate();
    /** Text Insert mode changed, called from statusbar */
    void slotOverwriteModeChange(int);
    /** Text Insert mode changed, called from Shortcut Key */
    void slotOverwriteModeChange();
    /** Toggles raw text/rich text edit mode */
    void slotRawTextMode();
    
    /** Shows requested popup menu for links */
    void slotContextLinksMenu(QListBoxItem* item, const QPoint&);
    /** show current attachment **/
    void slotShowAttachment(QListBoxItem* i);
    /** Add link **/
    void slotLinkAdd();
    /** Handle dropped link **/
    void slotLinkDropped(const QString& s, int flags);
    /** Remove link **/
    void slotLinkRemove();
    /** Open link **/
    void slotLinkOpen();
    /** Open link with selected application **/
    void slotLinkOpenWith();
    /** Modify link **/
    void slotLinkModify();
    /** Copy link location **/
    void slotLinkCopy();
};

#endif
