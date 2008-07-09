/***************************************************************************
                          knowit.cpp  -  main window
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

#include <qtimer.h>
#include <qclipboard.h>
#include <qdatetime.h>
#include <qdir.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qpainter.h>
#include <qpaintdevicemetrics.h>
#include <qwhatsthis.h>
#include <qsimplerichtext.h>
#include <qsplitter.h>

#include <kaction.h>
#include <kcmdlineargs.h>
#include <kcolordialog.h>
#include <kconfig.h>
#include <kdeversion.h>
#include <kedittoolbar.h>
#include <kfiledialog.h>
#include <kkeydialog.h>
#include <kmenubar.h>
#include <kmessagebox.h>
#include <kmimetype.h>
#include <kpopupmenu.h>
#include <kprinter.h>
#include <kstatusbar.h>
#include <ktip.h>
#include <kinputdialog.h>

#include "knowit.h"
#include "knowitapp.h"
#include "knowittray.h"
#include "knowittree.h"
#include "knowitlinks.h"

const QString Knowit::Untitled = i18n("untitled") + ".kno";

Knowit::Knowit(QWidget*, const char *name) : KMainWindow(0, name),
   FindDlg(), PrefDlg(), ChooserDlg(), LinkDlg(), Notes()
{

   statusBar()->insertItem(i18n("Ready"), 0);
   Layout = new QSplitter(this);
   Items = new KnowitTree(Layout, "Items");
   QSplitter* EditLayout = new QSplitter(Layout);
   EditLayout->setOrientation(QSplitter::Vertical);
   Edit = new KnowitEdit(EditLayout, "Edit");
   Links = new KnowitLinks(EditLayout);

   connect(&FindDlg, SIGNAL(search()), this, SLOT(slotEditFind()));
   lastSought = 0;

   /* main view */
   setAutoSaveSettings();
   setCentralWidget(Layout);

   QWhatsThis::add(Layout, i18n("Move it to change tree/edit layout."));
   connect(Items, SIGNAL(contextMenu(KListView*, QListViewItem*, const QPoint&)),
     this, SLOT(slotContextMenu(KListView*, QListViewItem*, const QPoint&)));
   connect(Items, SIGNAL(selectionChanged(QListViewItem*)), this,
     SLOT(slotItemChanged(QListViewItem*)));
   connect(Items, SIGNAL(selectionChanged(QListViewItem*)), this,
     SLOT(slotItemChanged(QListViewItem*)));

   /* Edit (QTextEdit) */
   Edit->setMinimumSize(200, 150);
   Edit->setTextFormat(Qt::RichText);
   QWhatsThis::add(Edit, i18n("<h2>Note text</h2>Add text for selected note here.\n"
      "Text can be formatted, for example <b>bold</b> or <i>italic</i>."));
   connect(Edit, SIGNAL(selectionChanged()), this, SLOT(slotEditChanged()));
   connect(Edit, SIGNAL(textChanged()), this, SLOT(slotEditChanged()));
   connect(Edit, SIGNAL(cursorPositionChanged(int, int)), this,
     SLOT(slotEditCursorChanged(int, int)));
   connect(Edit, SIGNAL(textDropped(const QString&, int)),
      this, SLOT(slotLinkDropped(const QString&, int)));

   /* Links (KListBox) */
   connect(Links, SIGNAL(doubleClicked(QListBoxItem*)),
      this, SLOT(slotShowAttachment(QListBoxItem*)));
   connect(Links, SIGNAL(contextMenuRequested(QListBoxItem*, const QPoint&)),
      this, SLOT(slotContextLinksMenu(QListBoxItem*, const QPoint&)));
   connect(Links, SIGNAL(textDropped(const QString&, int)),
      this, SLOT(slotLinkDropped(const QString&, int)));
   connect(Links, SIGNAL(linkRemove()), this, SLOT(slotLinkRemove()));
   connect(Links, SIGNAL(linkOpen()), this, SLOT(slotLinkOpen()));
   connect(Links, SIGNAL(linkOpenWith()), this, SLOT(slotLinkOpenWith()));

   /* Actions: File */
   KStdAction::openNew(this, SLOT(slotFileNew()), actionCollection());
   KStdAction::open(this, SLOT(slotFileOpen()), actionCollection());
   actionRecent = KStdAction::openRecent(this, SLOT(slotFileRecent(const KURL&)),
      actionCollection());


   KStdAction::save(this, SLOT(slotFileSave()), actionCollection());
   KStdAction::saveAs(this, SLOT(slotFileSaveAs()), actionCollection());
   new KAction(i18n("&Export to HTML..."), "", "", this,
      SLOT(slotFileExport()), actionCollection(), "file_export_html");
   KStdAction::print(this, SLOT(slotFilePrint()), actionCollection());
   new KAction(i18n("Document &information..."), "", "", this,
      SLOT(slotFileInfo()), actionCollection(), "file_info");
   KStdAction::quit(this, SLOT(slotFileQuit()), actionCollection());

   /* Actions: Edit */
   actionCut = KStdAction::cut(Edit, SLOT(cut()), actionCollection());
   actionCopy = KStdAction::copy(Edit, SLOT(copy()), actionCollection());
   KStdAction::paste(Edit, SLOT(paste()), actionCollection());
   actionUndo = KStdAction::undo(Edit, SLOT(undo()), actionCollection());
   actionRedo = KStdAction::redo(Edit, SLOT(redo()), actionCollection());
   actionBold = new KToggleAction(i18n("Toggle &bold"), "text_bold",
      "Ctrl+B", this, SLOT(slotEditBold()), actionCollection(), "edit_bold");
   actionItalic = new KToggleAction(i18n("Toggle &italic"), "text_italic",
      "Ctrl+I", this, SLOT(slotEditItalic()), actionCollection(),
      "edit_italic");
   actionUnderline = new KToggleAction(i18n("Toggle &underline"), "text_under",
      "Ctrl+U", this, SLOT(slotEditUnderline()), actionCollection(),
      "edit_underline");
   new KAction(i18n("&Text color..."), "color_line", "Ctrl+#", this,
      SLOT(slotEditColor()), actionCollection(), "edit_color");
   new KAction(i18n("&Superscript"), "text_super", "", this,
      SLOT(slotEditSuperscript()), actionCollection(), "edit_superscript");
   new KAction(i18n("&Subscript"), "text_sub", "", this,
      SLOT(slotEditSubscript()), actionCollection(), "edit_subscript");
   new KAction(i18n("&Normal text"), "", "", this,
      SLOT(slotEditNormal()), actionCollection(), "edit_align_normal");
   new KAction(i18n("&Bullet list"), "", "", this,
      SLOT(slotEditListBullet()), actionCollection(), "list_bullet");
   new KAction(i18n("&Numbered list"), "", "", this,
      SLOT(slotEditListNumber()), actionCollection(), "list_number");
   new KAction(i18n("&Uppercase list"), "", "", this,
      SLOT(slotEditListUpper()), actionCollection(), "list_upper");
   new KAction(i18n("&Lowercase list"), "", "", this,
      SLOT(slotEditListLower()), actionCollection(), "list_lower");
   new KAction(i18n("N&o list"), "", "", this,
      SLOT(slotEditListNone()), actionCollection(), "list_none");
   KStdAction::find(&FindDlg, SLOT(exec()), actionCollection());
   KStdAction::findNext(this, SLOT(slotEditFindNext()), actionCollection());
   new KAction(i18n("&Go to &note..."), "goto", "Ctrl+G", this,
      SLOT(slotEditGoTo()), actionCollection(), "goto_note");
   actionEditAlignLeft = new KToggleAction(i18n("Align &left"), "text_left", "Ctrl+L", this,
      SLOT(slotEditAlignLeft()), actionCollection(), "edit_align_left");
   actionEditAlignRight = new KToggleAction(i18n("Align &right"), "text_right", "Ctrl+R",
      this, SLOT(slotEditAlignRight()), actionCollection(), "edit_align_right");
   actionEditAlignJustify = new KToggleAction(i18n("&Justify"), "text_block", "Ctrl+J",
      this, SLOT(slotEditAlignJustify()), actionCollection(), "edit_align_justify");
   actionEditAlignCenter = new KToggleAction(i18n("&Center"), "text_center", "Ctrl+E", this,
      SLOT(slotEditAlignCenter()), actionCollection(), "edit_align_center");
   new KAction(i18n("Insert &date"), "date", "F5", this,
      SLOT(slotEditInsertDate()), actionCollection(), "edit_insert_date");
   new KAction(i18n("Insert &file..."), "", "", this,
      SLOT(slotEditInsertFile()), actionCollection(), "edit_insert_file");
   actionRawTextMode = new KToggleAction(i18n("&Raw Text Mode"),
       "source", "F10", this, SLOT(slotRawTextMode()),
       actionCollection(), "edit_raw_text");

   /* Actions: edit setup */
   actionCut->setEnabled(false);
   actionCopy->setEnabled(false);
   connect(Edit, SIGNAL(copyAvailable(bool)), actionCut, SLOT(setEnabled(bool)));
   connect(Edit, SIGNAL(copyAvailable(bool)), actionCopy, SLOT(setEnabled(bool)));
   connect(Edit, SIGNAL(undoAvailable(bool)), actionUndo, SLOT(setEnabled(bool)));
   connect(Edit, SIGNAL(redoAvailable(bool)), actionRedo, SLOT(setEnabled(bool)));

   /* Actions: notes */
   new KAction(i18n("&Add"), "", "Alt+Insert", this, SLOT(slotNoteAdd()),
      actionCollection(), "note_add");
   new KAction(i18n("Add &subnote"), "", "Alt+Shift+Insert", this,
      SLOT(slotNoteAddChild()), actionCollection(), "note_add_child");
   new KAction(i18n("&Delete"), "", "Alt+Delete", this,
      SLOT(slotNoteRemove()), actionCollection(), "note_delete");
   new KAction(i18n("&Rename"), "", "F2", this,
      SLOT(slotNoteRename()), actionCollection(), "note_rename");
   new KAction(i18n("E&xpand all"), "", "", Items,
      SLOT(slotExpandAll()), actionCollection(), "note_expand_all");
   new KAction(i18n("&Expand current"), "", "Alt++", Items,
      SLOT(slotExpand()), actionCollection(), "note_expand");
   new KAction(i18n("Co&llapse all"), "", "", Items,
      SLOT(slotCollapseAll()), actionCollection(), "note_collapse_all");
   new KAction(i18n("&Collapse current"), "", "Alt+-", Items,
      SLOT(slotCollapse()), actionCollection(), "note_collapse");
   new KAction(i18n("Move up"), "up", "Alt+Shift+Up", this,
      SLOT(slotNoteMoveUp()), actionCollection(), "note_move_up");
   new KAction(i18n("Move down"), "down", "Alt+Shift+Down", this,
      SLOT(slotNoteMoveDown()), actionCollection(), "note_move_down");
   new KAction(i18n("Move level up"), "back", "Alt+Shift+Left", this,
      SLOT(slotNoteMoveLeft()), actionCollection(), "note_move_left");
   new KAction(i18n("Move level down"), "forward", "Alt+Shift+Right", this,
      SLOT(slotNoteMoveRight()), actionCollection(), "note_move_right");
   new KAction(i18n("Move at the beginning"), "top", "Alt+Shift+Home", this,
      SLOT(slotNoteMoveBegin()), actionCollection(), "note_move_begin");
   new KAction(i18n("Move at the end"), "bottom", "Alt+Shift+End", this,
      SLOT(slotNoteMoveEnd()), actionCollection(), "note_move_end");
   new KAction(i18n("Sort"), "", "", this,
      SLOT(slotNoteSort()), actionCollection(), "note_sort");
   new KAction(i18n("Statistics..."), "", "", this,
      SLOT(slotNoteStatistics()), actionCollection(), "note_statistics");

   /* Actions: Links */
   new KAction(i18n("&Add link..."), "attach", "Ctrl+Shift+I", this,
      SLOT(slotLinkAdd()), actionCollection(), "link_add");
   new KAction(i18n("&Remove link"), "", "", this,
      SLOT(slotLinkRemove()), actionCollection(), "link_remove");
   new KAction(i18n("Open link"), "run", "", this,
      SLOT(slotLinkOpen()), actionCollection(), "link_open");
   new KAction(i18n("Open link with..."), "", "", this,
      SLOT(slotLinkOpenWith()), actionCollection(), "link_open_with");
   new KAction(i18n("&Modify link..."), "", "", this,
      SLOT(slotLinkModify()), actionCollection(), "link_modify");
   new KAction(i18n("&Copy link location"), "", "", this,
      SLOT(slotLinkCopy()), actionCollection(), "link_copy");

   /* Actions: Options */
   KStdAction::keyBindings(this, SLOT(slotOptionsKeys()), actionCollection());
   KStdAction::configureToolbars(this, SLOT(slotOptionsToolbar()), actionCollection());
   KStdAction::preferences(this, SLOT(slotOptions()), actionCollection());

   /* Actions: Help */
   new KAction(i18n("&Tip of the day"), "idea", "", this,
      SLOT(slotHelpTip()), actionCollection(), "help_tip");

   /* Actions: nonplugged */
   new KAction(i18n("Minimize"), "", "", this, SLOT(showMinimized()),
      actionCollection(), "window_minimize");
   new KAction(i18n("Switch area"), "", "Ctrl+M", this,
      SLOT(slotEditSwitch()), actionCollection(), "edit_switch");
   new KAction(i18n("Overwrite Mode"), "", "Insert", this,
      SLOT(slotOverwriteModeChange()), actionCollection(), "overwrite_mode_change");
   new KAction(i18n("Go to previous note"), "", "Alt+Up", Items,
      SLOT(slotItemUp()), actionCollection(), "note_previous");
   new KAction(i18n("Go to next note"), "", "Alt+Down", Items,
      SLOT(slotItemDown()), actionCollection(), "note_next");
   new KAction(i18n("Go to first note"), "", "Alt+Home", Items,
      SLOT(slotItemBegin()), actionCollection(), "note_begin");
   new KAction(i18n("Go to last note"), "", "Alt+End", Items,
      SLOT(slotItemEnd()), actionCollection(), "note_end");
   new KAction(i18n("Go to first subnote"), "", "Alt+Right", Items,
      SLOT(slotItemRight()), actionCollection(), "note_right");
   new KAction(i18n("Go to parent note"), "", "Alt+Left", Items,
      SLOT(slotItemLeft()), actionCollection(), "note_left");

   /* Actions */
   setStandardToolBarMenuEnabled(true);
   createGUI("knowitui.rc", false);


   /* Insert/overwrite mode */
   statusBar()->insertFixedItem(("WWW"), StatusOvr, true);
   statusBar()->changeItem(i18n("INS"), StatusOvr);
   connect(statusBar(), SIGNAL(pressed(int)), this,
      SLOT(slotOverwriteModeChange(int)));

   /* System tray */
   systray = new KnowitTray(this, "Systray");
   systray->setPixmap(Notes.Pixmaps[1]);
   systray->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

   /* Autosave */
   AutosaveTimer = new QTimer(this);
   connect(AutosaveTimer, SIGNAL(timeout()), this, SLOT(slotFileAutosave()));

   /* Configuration */
   config = kapp->config();
   readOptions();

   if (runMinimized)
      showMinimized();
   else 
       show();

   bool opened = false;
   KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
   for(int i = 0; i < args->count(); i++)
       if (open(args->url(i))) {
          opened = true; break;
          }
   args->clear();

   if (!opened && Options.reopen && filename.path() != "")
      open(KURL(filename));

   slotActionUpdate();

   KTipDialog::showTip(this);

   shuttingDown = false;

   applyMainWindowSettings(config);

   /* check if knowituirc was found */
   QWidget *w = factory()->container("notes_popup", this);
   if (!w)
      KMessageBox::error(0, i18n("<qt><p>Cannot open resource file <b>knowitui.rc</b>. "
          "Some of the menus and toolbars will be missing.</p>"
          "<p>Probably Knowit was not installed properly. "
    "If you installed from source, please invoke <i>make install</i>."
    "If you use precompiled package, please contact packager.</p></qt>"));
}

Knowit::~Knowit()
{
   delete systray;
   delete AutosaveTimer;
}

bool Knowit::open(const KURL& fname)
{
   QFile file(fname.path());
   if (!file.open(IO_ReadOnly)) {
      KMessageBox::error(0, i18n("<qt>Cannot open file<br><b>%1</b></qt>")
         .arg(fname.url()));
      if (filename == fname)
         filename = "";
      return false;
      }
   uint prevdepth = 0;
   reset();
   QTextStream ts(&file);
   ts.setEncoding(QTextStream::UnicodeUTF8);
   QString s = "";
   QListViewItem* current = 0;
   QListViewItem* active = 0;
   Notes.autoUpdate = false;
   while (!s.isNull()) {
      s = ts.readLine();
      if (s.left(9) == "\\NewEntry" || s.left(13) == "\\CurrentEntry") {
         QString name;
         uint depth;
         QTextIStream ls(&s);
         ls >> name >> depth;
         name = ls.readLine();
         name = name.stripWhiteSpace();
         if (depth == prevdepth) /* same level */
            if (depth) current = new KListViewItem(current->parent(), current, name);
            else current = new KListViewItem(Items, current, name);
         else if (depth > prevdepth)   /* subnote */
            current = new KListViewItem(current, name);
         else {   /* parent note */
            for(uint i=depth; i<prevdepth && current; i++)
               current = current->parent();
            if (current && current->parent())
               current = new KListViewItem(current->parent(), current, name);
            else current = new KListViewItem(Items, current, name);
         }
         prevdepth = depth;
         Notes.addNote(current);
         Notes.find(current)->open(ts);
         if (s.left(13) == "\\CurrentEntry")
            active = current;
      }
   }
   Notes.autoUpdate = true;
   Notes.updateNotesView();
   filename = fname;
   setCaption(fname.fileName());
   Notes.modified = false;
   Edit->clear();
   slotNoteChanged(active);
   Edit->setModified(false);
   actionRecent->addURL(filename);
   slotStatusMsg(i18n("File %1 opened.").arg(filename.fileName()));
   slotActionUpdate();

   file.close();
   if (Options.backup) 
     URLOperator.copy(fname.path(), fname.path() + "~", false, false);
   if (Options.autosave)
     AutosaveTimer->start(Options.autosave * 60 * 1000, true);
   return true;
}

bool Knowit::save(const KURL& fname)
{
  QFile file(fname.path());
  if (!file.open(IO_WriteOnly)) {
     KMessageBox::error(0, i18n("<qt>Cannot save file<br><b>%1</b></qt>")
         .arg(fname.url()));
     return false;
     }
  if (Items->prevItem && Edit->isModified())
     Notes.changeNote(Items->prevItem, Edit->length() ? Edit->text() : QString::null);

  QTextStream ts(&file);
  ts.setEncoding(QTextStream::UnicodeUTF8);
  for (QListViewItemIterator it(Items); it.current(); it++)
     Notes.find(it.current())->save(ts, it.current() == Items->currentItem());
  filename = fname;
  setCaption(fname.fileName());
  Notes.modified = false;
  Edit->setModified(false);
  actionRecent->addURL(filename);
  slotStatusMsg(i18n("File %1 saved.").arg(filename.fileName()));
  if (Options.autosave)
     AutosaveTimer->start(Options.autosave * 60 * 1000, true);
  return true;
}

void Knowit::reset()
{
   Items->prevItem = 0;
   Notes.clearNotes();
   Items->clear();
   Edit->clear();
   filename = "";
   setCaption(Untitled);
   Notes.modified = false;
   slotActionUpdate();
}



void Knowit::closeEvent(QCloseEvent* e)
{
   if (!Options.docked || ((KnowitApplication*)kapp)->closeByLogout()) {
      saveOptions();
      KMainWindow::closeEvent(e);
      }
   else
      hide();
}

bool Knowit::queryClose()
{
   if (!modified()) return shuttingDown = true;
   else if (Options.unconditionalSave) {
       slotFileSave();
       return shuttingDown = filename.isEmpty() || !modified()  || (KMessageBox::questionYesNo(0,
           i18n("<qt>File <b>%1</b><br>\ncannot be saved. Quit anyway?</qt>").arg(filename.fileName())) 
	   == KMessageBox::Yes);
       }
   else
      switch (KMessageBox::questionYesNoCancel(0,
       i18n("<qt>File <b>%1</b><br>\nwas modified. Do you want to save it?</qt>")
          .arg(filename.isEmpty() ? Untitled : filename.fileName()))) {
      case KMessageBox::Yes:
         slotFileSave();
         return shuttingDown = !modified();
      case KMessageBox::No:
         return shuttingDown = true;
      default:
         return false;
      }
}

bool Knowit::queryExit()
{
   if (!shuttingDown) {
      /* brutally closed by session manager */
      if (modified())
         save(filename.isEmpty() ? i18n("untitled.kno") : filename);
      saveOptions();
   }
   return true;
}


void Knowit::find(QListViewItem* start)
{
   if (soughtText.isEmpty() || !Items->firstChild()) return;
   if (start && lastSought == start &&
      Edit->find(soughtText, FindDlg.case_sensitive(), false))
      return;
   if (!start) start = Items->firstChild();
   QListViewItemIterator it(start);
   while (it.current()) {
      QString s = Notes.text(it.current());
      if (lastSought != it.current() &&
         s.contains(soughtText, FindDlg.case_sensitive())) {
         slotNoteChanged(it.current());
         Edit->find(soughtText, FindDlg.case_sensitive(), false);
         lastSought = it.current();
         return;
      }
      it++;
   }
   KMessageBox::information(0, i18n("<qt>Sought text:<br><b>%1</b><br>not found.</qt>").arg(soughtText));
}


void Knowit::saveOptions()
{
  config->setGroup("MainWindow");
  config->writeEntry("Widgets", Layout->sizes());
  Options.save(config);
  config->setGroup("General");
  config->writeEntry("Minimized", isMinimized() || !isVisible());
  config->writeEntry("LastFile", filename.path());
  config->writeEntry("Geometry", size());
  actionCollection()->writeShortcutSettings("Keys", config);
  config->deleteGroup("Recent Files");
  actionRecent->saveEntries(config, "Recent Files");
  saveMainWindowSettings(config);
}

void Knowit::readOptions()
{
   config->setGroup("MainWindow");
   QValueList<int> sizes = config->readIntListEntry("Widgets");
   if (sizes.count() == 2)
      Layout->setSizes(sizes);
   Options.read(config);
   config->setGroup("General");
   runMinimized = config->readBoolEntry("Minimized", false);
   QString oldfile = config->readEntry("LastFile");
   QSize defSize = size();
   resize(config->readSizeEntry("Geometry", &defSize));
   actionCollection()->readShortcutSettings("Keys", config);
   actionRecent->loadEntries(config,"Recent Files");
   if (!oldfile.isEmpty())
      filename = KURL(oldfile);
   applyOptions(Options);
}

void Knowit::slotNoteChanged(QListViewItem* item)
{
   if (item) {
      if (Options.autoCollapse)
         Items->slotCollapseAll();
      Items->setCurrentItem(item);
      Items->setSelected(item, true);
      Items->ensureItemVisible(item);
   }
}

bool Knowit::modified()
{
   return Notes.modified || (Items->currentItem() && Edit->isModified());
}

void Knowit::applyOptions(const KnowitOptions& O, bool store)
{
   if (store) Options = O;
   if (O.docked)
      systray->show();
   else
      systray->hide();

   if (O.customColors) {
      Edit->setPaletteForegroundColor(QColor(Options.foreEditColor));
      Edit->setPaletteBackgroundColor(QColor(Options.backEditColor));
   }
   else Edit->unsetPalette();

   if (Options.customFont)
      Edit->setFont(QFont(Options.fontFamily, Options.fontSize));
   else
      Edit->unsetFont();

   Edit->setWordWrap(O.wordwrap ? QTextEdit::WidgetWidth : QTextEdit::NoWrap);
   Edit->setTabChangesFocus(O.tabfocus);
   if (!O.autosave)
       AutosaveTimer->stop();
   else
       AutosaveTimer->start(O.autosave * 60 * 1000, true);
   Edit->swapEOL = O.enterBreakLine;
   Layout->setOrientation(O.horizontalSplit ? QSplitter::Horizontal :
         QSplitter::Vertical);
   Items->setAlternateBackground(O.alternateTree ? QColor(O.alternateColor) : QColor());
   Items->repaintContents();
   if (Items->currentItem())
      slotItemChanged(Items->currentItem());
   slotNoteChanged(Items->currentItem());
}

TNote* Knowit::currentNote() const
{
   if (!Items->currentItem()) return 0;
   return Notes.find(Items->currentItem());
}






/*** Slots ***/
void Knowit::slotStatusMsg(const QString &text)
{
   statusBar()->clear();
   statusBar()->changeItem(text, 0);
}


void Knowit::slotContextMenu(KListView*, QListViewItem*, const QPoint& p)
{
   QWidget *w = factory()->container("notes_popup", this);
   if (w)
      static_cast<QPopupMenu *>(w)->exec(p);
}


void Knowit::slotContextLinksMenu(QListBoxItem*, const QPoint& p)
{
   QWidget *w = factory()->container("links_popup", this);
   if (w)
      static_cast<QPopupMenu *>(w)->exec(p);
}



/******* Slots: Notes *******/
void Knowit::slotNoteAdd()
{
   bool Ok;
   KListViewItem* elt;
   QString text = KInputDialog::getText(i18n("Add note"),
      i18n("Note title:"), Options.defaultName, &Ok);
   if (!Ok) return;
   if (Items->currentItem())
      if (Items->currentItem()->parent())
         elt = new KListViewItem(Items->currentItem()->parent(),
            Items->currentItem(), text);
      else
         elt = new KListViewItem(Items, Items->currentItem(), text);
   else
      elt = new KListViewItem(Items, text);
   Notes.addNote(elt);
   slotNoteChanged(elt);
   if (Notes.count() == 1)
      slotActionUpdate();
}

void Knowit::slotNoteAddChild()
{
   if (!Items->currentItem()) {
      KMessageBox::error(0, i18n("You have to choose note before adding subnote."));
      return;
   }
   bool Ok;
   QString text = KInputDialog::getText(i18n("Add subnote"),
      i18n("Note title:"), Options.defaultName, &Ok);
   if (!Ok) return;
   QListViewItem* lastChild = Items->currentItem()->firstChild();
   while (lastChild && lastChild->nextSibling())
      lastChild = lastChild->nextSibling();

   KListViewItem* elt;
   if (lastChild)
       elt = new KListViewItem(Items->currentItem(), lastChild, text);
   else
      elt = new KListViewItem(Items->currentItem(), text);
   Notes.addNote(elt);
   slotNoteChanged(elt);
}

void Knowit::slotNoteRemove()
{
   QListViewItem* elt = Items->currentItem();
   if (!elt) return;
   QString msg = elt->childCount() ?
      i18n("<qt>Are you sure you want to delete note<br><b>%1</b><br> "
         "and its subnotes?</qt>") :
      i18n("<qt>Are you sure you want to delete note<br><b>%1</b>?</qt>");
   if (KMessageBox::questionYesNo(0, msg.arg(elt->text(0))) == KMessageBox::Yes) {
      QListViewItem* parent = elt->parent();
      Notes.removeNote(elt);
      if (!Notes.count())
         slotActionUpdate();
      if (!parent)
         parent = Items->firstChild();
      Items->prevItem = 0;
      slotNoteChanged(parent);
      }
}

void Knowit::slotNoteRename()
{
    if (Items->currentItem()) {
       Items->currentItem()->setRenameEnabled(0, true);
       Items->currentItem()->startRename(0);
       }
}

void Knowit::slotNoteStatistics()
{
  int depth = 0;
  int words = TNotesCollection::wordCount(Edit->text());
  int chars = TNotesCollection::charCount(Edit->text());
  int pars = TNotesCollection::parsCount(Edit->text());
  int allwords = 0;
  int allpars = 0;
  int allchars = 0;
  for (QListViewItemIterator it(Items); it.current(); ++it)
  {
    if (!(*it)->childCount() && !(*it)->nextSibling() && (*it)->depth() > depth)
      depth = (*it)->depth();
  }
  for (QPtrDictIterator<TNote> it(Notes); it.current(); ++it)
  {
    allwords += TNotesCollection::wordCount((*it).text);
    allpars += TNotesCollection::parsCount((*it).text);
    allchars += TNotesCollection::charCount((*it).text);
  }
  KMessageBox::information(0, i18n("<qt><br><b>Current note:</b><br>"
  "Characters: %1<br>Words: %2<br>Paragraphs: %3<br><br>"
  "<b>Current file:</b><br>Number of notes: %4<br>Maximum depth: %5<br>"
  "Characters: %6<br>Words: %7<br>Paragraphs: %8</qt>")
    .arg(chars).arg(words).arg(pars)
    .arg(Notes.count()).arg(depth).arg(allchars).arg(allwords).arg(allpars),
    i18n("Statistics"));
}




void Knowit::slotItemChanged(QListViewItem* i)
{
   if (Items->prevItem && Edit->isModified())
      Notes.changeNote(Items->prevItem, Edit->length() ? Edit->text() : QString());
   Items->prevItem = i;
   QString s;
   if (!Items->prevItem || (s = Notes.text(Items->prevItem)).isEmpty())
      Edit->setText(QString::null);
   else
      Edit->setText(s);
   Edit->setModified(false);
   Links->clear();
   TNote* note = Notes.find(i);
   if (note)
      for (int j=0; j<note->linkCount(); j++)
         Links->insertItem(note->link(j).icon(), note->link(j).text(Options.linkFormat));
   if (Links->count())
      Links->show();
   else
      Links->hide();
}

void Knowit::slotNoteMoveUp()
{
   QListViewItem* elt = Items->currentItem();
   if (elt && elt->itemAbove()) {
      QListViewItem* above = elt->parent() ? elt->parent()->firstChild()
         : Items->firstChild();
      if (above == elt)
         return;
      if (!above->nextSibling() || above->nextSibling() == elt)
         above = 0;
      else while (above->nextSibling()->nextSibling() != elt)
         above = above->nextSibling();
      if (above) 
         elt->moveItem(above);
      else {
         QListViewItem* parent = elt->parent();
         Items->takeNode(elt);
         Items->insertNode(parent, elt);
      }
      slotNoteChanged(elt);
      Notes.modified = true;
   }
}

void Knowit::slotNoteMoveDown()
{
   QListViewItem* elt = Items->currentItem();
   if (elt && elt->nextSibling()) {
      elt->moveItem(elt->nextSibling());
      slotNoteChanged(elt);
      Notes.modified = true;
      }
}

void Knowit::slotNoteMoveBegin()
{
   QListViewItem* elt = Items->currentItem();
   if (elt) {
      QListViewItem* parent = elt->parent();
      Items->takeNode(elt);
      Items->insertItem(elt);
      if (parent)
         Notes.find(parent)->updateView();
      slotNoteChanged(elt);
      Notes.modified = true;
      }
}

void Knowit::slotNoteMoveEnd()
{
   QListViewItem* elt = Items->currentItem();
   if (!elt)
      return;
   QListViewItem* last = Items->firstChild();
   QListViewItem* parent = elt->parent();
   while (last->nextSibling())
      last = last->nextSibling();
   if (elt && elt != last) {
      Items->takeNode(elt);
      Items->insertNode(0, elt, last);
      slotNoteChanged(elt);
      if (parent) 
         Notes.find(parent)->updateView();
      Notes.modified = true;
      }
}

void Knowit::slotNoteMoveLeft()
{
   QListViewItem* elt = Items->currentItem();
   QListViewItem* parent = elt->parent();
   if (elt && parent) {
      Items->takeNode(elt);
      Items->insertNode(parent->parent(), elt, parent);
      Notes.find(parent)->updateView();
      slotNoteChanged(elt);
      Notes.modified = true;
      }
}

void Knowit::slotNoteMoveRight()
{
   QListViewItem* elt = Items->currentItem();
   QListViewItem* above = elt->itemAbove();
   if (elt && above) {
      if (above->depth() < elt->depth())
         return;
      else if (above->depth() == elt->depth()) {
         Items->takeNode(elt);
	 Items->insertNode(above, elt);
	 }
      else {
         while (above->depth() > elt->depth()+1)
            above = above->parent();
         Items->takeNode(elt);
         Items->insertNode(above->parent(), elt, above);
	 }
      Notes.find(above)->updateView();
      slotNoteChanged(elt);
      Notes.modified = true;
   }
}

void Knowit::slotNoteSort()
{
   if (Items->currentItem()) {
      Items->currentItem()->sortChildItems(0, true);
      Items->repaintContents();
   }
}



void Knowit::slotFileNew()
{
   if (queryClose()) reset();
}

void Knowit::slotFileOpen()
{
   if (!queryClose()) return;
   KURL url=KFileDialog::getOpenURL(QString::null,
      i18n("*.kno|KnowIt files (*.kno)\n*|All files"), this, i18n("Open File..."));
   if (!url.isEmpty())
      open(url);
}

void Knowit::slotFileSave()
{
  if (filename.isEmpty())
     slotFileSaveAs();
  else {
     save(filename);
     }
}

void Knowit::slotFileAutosave()
{
  if (modified()) 
     slotFileSave();
  if (modified()) {	/* file wasn't saved */
     KMessageBox::sorry(0, i18n("<qt><p>File couldn not be saved automatically."
          "<p>Autosaving disabled - save file manually to turn autosaving on.</p></qt>"));
     AutosaveTimer->stop();
     }
}


void Knowit::slotFileRecent(const KURL& url)
{
   if (queryClose())
      if (!open(url))
         actionRecent->removeURL(url);
}


void Knowit::slotFileSaveAs()
{
   KURL url=KFileDialog::getSaveURL(QDir::currentDirPath(),
        i18n("*.kno|KnowIt files (*.kno)\n*|All files"), this, i18n("Save as..."));
   if (!url.isEmpty()) {
      if (!url.fileName().contains('.'))
        url = KURL(url.path() + ".kno");
      QFileInfo fileinfo(url.path());
      if (fileinfo.exists() && KMessageBox::questionYesNo(0,
         i18n("<qt>File<br><b>%1</b><br>already exists. Overwrite it?</qt>")
            .arg(url.path())) == KMessageBox::No)
         return;
      save(url);
  }
}


void Knowit::slotFileInfo()
{
   int count = 0;
   QListViewItemIterator it(Items);
   while (it.current()) {
      count++;
     it++;
   }
   KMessageBox::information(0, i18n("<qt><h1>%1</h1>"
      "Document path: %2<br>"
      "Number of notes: %3</qt>").arg(filename.fileName())
         .arg(filename.path()).arg(count));
}


void Knowit::slotFilePrint()
{
   KPrinter printer;
   printer.setFullPage(true);
   if (Edit->text().stripWhiteSpace().isEmpty() || !printer.setup(this))
      return;

   QSimpleRichText richText(Edit->text(), Edit->font());
   QPainter p(&printer);
   QPaintDeviceMetrics metrics(p.device());
   int dpix = metrics.logicalDpiX();
   int dpiy = metrics.logicalDpiY();
   const int margin = 72; // pt
   QRect body(margin*dpix / 72, margin*dpiy / 72,
      metrics.width()-  margin*dpix /72*2,
      metrics.height()-margin*dpiy/72*2);
   richText.setWidth(&p, body.width());
   QRect view(body);
   int page = 1;
   while (true) {
      richText.draw(&p, body.left(), body.top(), view, colorGroup());
         view.moveBy(0, body.height());
      p.translate( 0 , -body.height());
      p.setFont(Edit->font());
      p.drawText( view.right() - p.fontMetrics().width( QString::number(page)),
         view.bottom() + p.fontMetrics().ascent() + 5, QString::number(page));
      if (view.top() >= body.top() + richText.height())
         break;
      printer.newPage();
      page++;
   }
}




void Knowit::slotFileQuit()
{
   if (queryClose()) {
      saveOptions();
      kapp->quit();
      }
}

void Knowit::slotFileExport()
{
  if (!Items->firstChild()) return;         /* nothing to export */
  ChooserDlg.setChoice(Options.exportFlags);
  if (ChooserDlg.exec() != QDialog::Accepted) return; /* cancelled */

  /* Deduce filename from Knowit document name */
  QString htmlfile;
  if (filename.isValid()) {
     htmlfile = filename.path();
     if (htmlfile.right(4) == ".kno")
        htmlfile = htmlfile.left(htmlfile.length() - 4) + ".html";
     else htmlfile = "";
  }
  if (htmlfile.isEmpty())
     htmlfile == QDir::currentDirPath();

  /* Confirm/choose filename */
  KURL url=KFileDialog::getSaveURL(htmlfile,
        i18n("*.html|HTML files (*.html)\n*|All files"), this, i18n("Export to HTML"));
  if (!url.isEmpty()) {
     if (!url.fileName().contains('.'))
        url = KURL(url.path() + ".html");
     QFileInfo fileinfo(url.path());
     if (fileinfo.exists() && KMessageBox::questionYesNo(0,
         i18n("<qt>File<br><b>%1</b><br>already exists. Overwrite it?</qt>")
            .arg(url.path())) == KMessageBox::No)
        return;
     int choice = ChooserDlg.getChoice();
     QListViewItem* start = (choice & KnowitChooser::SaveAll) ?
         Items->firstChild() : Items->currentItem();
     QString origname = filename.isEmpty() ? Untitled : filename.path();
     QString style;
     if (choice & KnowitChooser::Style)
        style = QString("body {font-family: \"%1\"; color: %2; background-color: %3}")
           .arg(Edit->family()).arg(Edit->paletteForegroundColor().name())
           .arg(Edit->paletteBackgroundColor().name());
     Notes.find(start)->saveHTML(url, origname, style, choice);
     Options.exportFlags = choice;
  }
}

void Knowit::slotEditBold()
{
   Edit->setBold(actionBold->isChecked());
}

void Knowit::slotEditItalic()
{
   Edit->setItalic(actionItalic->isChecked());
}

void Knowit::slotEditUnderline()
{
   Edit->setUnderline(actionUnderline->isChecked());
}

void Knowit::slotEditColor()
{
   QColor color = Edit->color();
   if (KColorDialog::getColor(color) == QDialog::Accepted)
      Edit->setColor(color);
}

void Knowit::slotEditSuperscript()
{
   Edit->setVerticalAlignment(QTextEdit::AlignSuperScript);
}

void Knowit::slotEditSubscript()
{
   Edit->setVerticalAlignment(QTextEdit::AlignSubScript);
}

void Knowit::slotEditNormal()
{
   Edit->setVerticalAlignment(QTextEdit::AlignNormal);
}

void Knowit::slotEditAlignLeft()
{
   Edit->setAlignment(Qt::AlignLeft);
}

void Knowit::slotEditAlignRight()
{
   Edit->setAlignment(Qt::AlignRight);
}

void Knowit::slotEditAlignJustify()
{
   Edit->setAlignment(Qt::AlignJustify);
}

void Knowit::slotEditAlignCenter()
{
   Edit->setAlignment(Qt::AlignCenter);
}

void Knowit::slotEditListBullet()
{
   Edit->setParagType(QStyleSheetItem::DisplayListItem, QStyleSheetItem::ListDisc);
}

void Knowit::slotEditListNumber()
{
   Edit->setParagType(QStyleSheetItem::DisplayListItem, QStyleSheetItem::ListDecimal);
}

void Knowit::slotEditListUpper()
{
   Edit->setParagType(QStyleSheetItem::DisplayListItem, QStyleSheetItem::ListUpperAlpha);
}

void Knowit::slotEditListLower()
{
   Edit->setParagType(QStyleSheetItem::DisplayListItem, QStyleSheetItem::ListLowerAlpha);
}

void Knowit::slotEditListNone()
{
   Edit->setParagType(QStyleSheetItem::DisplayBlock, QStyleSheetItem::ListDisc);
}

void Knowit::slotEditFind()
{
   if (soughtText != FindDlg.getText()) {
      soughtText = FindDlg.getText();
      lastSought = 0;
      find();
      }
   else
      find(Items->currentItem());
}

void Knowit::slotEditFindNext()
{
   if (soughtText.isEmpty()) FindDlg.exec();
   find(Items->currentItem());
}

void Knowit::slotEditGoTo()
{
   bool Ok;
   QString page = KInputDialog::getText(i18n("Go to"),
      i18n("Go to page with given title"), "", &Ok);
   if (Ok && !page.isEmpty()) {
      QListViewItem* item = Items->findItem(page, 0);
      if (!item) item = Items->findItem(page, 0, Qt::ExactMatch);
      if (!item) item = Items->findItem(page, 0, Qt::Contains);
      slotNoteChanged(item);
   }
}

void Knowit::slotEditInsertDate()
{
   if (Items->currentItem()) {
      bool ib = actionBold->isChecked(), ii=actionItalic->isChecked(), iu=actionUnderline->isChecked();
      QColor color = Edit->color();
      Edit->setItalic(Options.insertDateItalic);
      Edit->setBold(Options.insertDateBold);
      Edit->setUnderline(Options.insertDateUnderline);
      Edit->setColor(QColor(Options.insertDateColor));
      Edit->insert(QString("%1")
         .arg(QDateTime::currentDateTime().toString(Options.insertDateFormat)));
      Edit->setItalic(ii);
      Edit->setBold(ib);
      Edit->setUnderline(iu);
      Edit->setColor(color);

   }
}

void Knowit::slotEditInsertFile()
{
   if (!Items->currentItem())
      return;
   KURL url=KFileDialog::getOpenURL(QString::null,
      i18n("*|All files"), this, i18n("Insert file..."));
   if (url.isEmpty())
      return;
   QFile file(url.path());
   if (!file.open(IO_ReadOnly)) {
      KMessageBox::error(0, i18n("<qt>Cannot open file<br><b>%1</b></qt>")
         .arg(url.url()));
      return;
      }
   QTextStream ts(&file);
   ts.setEncoding(QTextStream::UnicodeUTF8);
   QString s, text;
   while (true) {
      s = ts.readLine();
      if (s.isNull())
         break;
      if (s.isEmpty())
         text += '\n';
      else
         text += s;
      }
   Edit->insert(text);
}


void Knowit::slotEditSwitch()
{
   if (Items->hasFocus())
      Edit->setFocus();
   else Items->setFocus();
}

void Knowit::slotEditChanged()
{
   actionBold->setChecked(Edit->bold());
   actionItalic->setChecked(Edit->italic());
   actionUnderline->setChecked(Edit->underline());
   actionEditAlignLeft->setChecked(Edit->alignment() == Qt::AlignLeft);
   actionEditAlignRight->setChecked(Edit->alignment() == Qt::AlignRight);
   actionEditAlignCenter->setChecked(Edit->alignment() == Qt::AlignCenter);
   actionEditAlignJustify->setChecked(Edit->alignment() == Qt::AlignJustify);
}

void Knowit::slotEditCursorChanged(int, int)
{
   slotEditChanged();
}



void Knowit::slotOptions()
{
   PrefDlg.setOptions(Options);
   if (PrefDlg.exec() == QDialog::Accepted)
      applyOptions(PrefDlg.getOptions());
}

void Knowit::slotOptionsKeys()
{
   KKeyDialog K(false);
   K.configure(actionCollection());
}

void Knowit::slotOptionsToolbar()
{
   KEditToolbar dlg(actionCollection());
   if (dlg.exec())
      createGUI();
}


void Knowit::slotUndock()
{
   Options.docked = false;
   applyOptions(Options);
}

void Knowit::slotHelpTip()
{
   KTipDialog::showTip(this, 0, true);
}


void Knowit::slotActionUpdate()
{
   if (Items->childCount())
      stateChanged("notesPresent");
   else
      stateChanged("notesPresent", KXMLGUIClient::StateReverse);
   Edit->setEnabled(Notes.count());
}

void Knowit::slotOverwriteModeChange(int i)
{
   if (i == StatusOvr)
      slotOverwriteModeChange();
}

void Knowit::slotOverwriteModeChange()
{
   Edit->setOverwriteMode(!Edit->isOverwriteMode());
   statusBar()->changeItem(Edit->isOverwriteMode() ? i18n("OVR") :
      i18n("INS"), StatusOvr);
}

void Knowit::slotRawTextMode()
{
   if (!Items->currentItem())
      return;
   if (Items->prevItem && Edit->isModified())
      Notes.changeNote(Items->prevItem, Edit->length() ? Edit->text() : QString());
   Edit->setTextFormat(actionRawTextMode->isChecked() ?
      Qt::PlainText : Qt::RichText);
   Edit->setText(Notes.text(Items->currentItem()));
}

void Knowit::slotShowAttachment(QListBoxItem*)
{
   slotLinkOpen();
}

void Knowit::slotLinkAdd()
{
   slotLinkDropped("", 0);
}

void Knowit::slotLinkDropped(const QString& s, int flags)
{
   TNoteLink link;
   link.link = s;
   if (!flags && !LinkDlg.modifyLink(link))
      return;
   currentNote()->addLink(link);
   Links->insertItem(link.icon(), link.text(Options.linkFormat));
   if (Links->count() == 1)
      Links->show();
   if (Options.firstLink) {
      KMessageBox::information(0, i18n("You have just added your first link. "
      "Please remember that only links are stored in Knowit document, "
      "not linked files. If you move your document to other computer, "
      "links to local files probably won't work."));
      Options.firstLink = false;
   }
}

void Knowit::slotLinkRemove()
{
   QString msg = i18n("<qt>Are you sure you want to remove link:<br><b>%1</b>?</qt>");
   if (Links->currentItem() != -1 && KMessageBox::questionYesNo(0,
      msg.arg(Links->currentText())) == KMessageBox::Yes) {
      currentNote()->removeLink(Links->currentItem());
      Links->removeItem(Links->currentItem());
      if (!Links->count())
         Links->hide();
      }
}

void Knowit::slotLinkOpen()
{
   if (Links->currentItem() == -1)
       return;
   TNoteLink link = currentNote()->link(Links->currentItem());
   if (link.isLocalReference()) {
      QString s = link.link.remove(0, 9);
      QListViewItem* item = Items->findItem(s, 0);
      if (!item) Items->findItem(s, 0, Qt::BeginsWith);
      if (!item) Items->findItem(s, 0, Qt::Contains);
      slotNoteChanged(item);
   }
   else
      link.open();
}

void Knowit::slotLinkOpenWith()
{
   if (Links->currentItem() != -1)
       currentNote()->link(Links->currentItem()).openWith();
}

void Knowit::slotLinkModify()
{
   if (Links->currentItem() == -1) return;
   TNoteLink link = currentNote()->link(Links->currentItem());
   if (LinkDlg.modifyLink(link)) {
      currentNote()->modifyLink(Links->currentItem(), link);
      Links->changeItem(link.icon(), link.text(Options.linkFormat),
         Links->currentItem());
      }
}

void Knowit::slotLinkCopy()
{
   if (Links->currentItem() == -1) return;
   TNoteLink link = currentNote()->link(Links->currentItem());
   kapp->clipboard()->setText(link.link, QClipboard::Clipboard);
   kapp->clipboard()->setText(link.link, QClipboard::Selection);
}

