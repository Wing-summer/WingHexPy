#include "scriptwindow.h"
#include "../WingHexExplorer/WingHexExplorer/plugin/iwingplugin.h"
#include "QCodeEditor/QPythonHighlighter.hpp"
#include "plginterface.h"
#include <DFileDialog>
#include <DInputDialog>
#include <DMessageManager>
#include <DTitlebar>
#include <DWidgetUtil>
#include <Python.h>
#include <QApplication>
#include <QMessageBox>
#include <QScrollBar>
#include <QShortcut>
#include <QTextDocumentFragment>

#define ICONRES(name) QIcon(":/WingHexPy/img/" name ".png")
#define HOSTICONRES(name) QIcon(HOSTRESPIMG(name))

ScriptWindow *ScriptWindow::m_instance = nullptr;

ScriptWindow *ScriptWindow::instance() {
  if (m_instance == nullptr) {
    m_instance = new ScriptWindow;
  }
  return m_instance;
}

ScriptWindow::ScriptWindow(DMainWindow *parent) : DMainWindow(parent) {
  setMinimumSize(QSize(800, 600));

  auto _title = titlebar();
  picon = ICONRES("pys");
  setWindowIcon(picon);
  _title->setIcon(picon);
  _title->setTitle(tr("WingHexPyScriptWindow"));
  auto w = new QWidget(this);
  setCentralWidget(w);
  vlayout = new QVBoxLayout(w);

  m_styles[0] = QSyntaxStyle::defaultStyle();
  auto darkstyle = new QSyntaxStyle(this);
  QFile file(":/WingHexPy/drakula.xml");
  if (file.open(QFile::ReadOnly)) {
    darkstyle->load(file.readAll());
    file.close();
  }
  auto guihelper = DGuiApplicationHelper::instance();
  connect(guihelper, &DGuiApplicationHelper::themeTypeChanged, this,
          &ScriptWindow::setTheme);

  m_styles[1] = darkstyle;
  editor = new QCodeEditor(this);
  editor->setHighlighter(new QPythonHighlighter);
  editor->setSyntaxStyle(
      m_styles[guihelper->themeType() == DGuiApplicationHelper::LightType ? 0
                                                                          : 1]);
  editor->setWordWrapMode(QTextOption::NoWrap);
  editor->setTabReplace(true);
  editor->setAutoIndentation(true);

  vlayout->addWidget(editor);

  auto scrun = QKeySequence(Qt::Key_F5);
  auto scrunf =
      QKeySequence(Qt::KeyboardModifier::ControlModifier | Qt::Key_F5);
  auto scgoto = QKeySequence(Qt::KeyboardModifier::ControlModifier | Qt::Key_G);

  PluginMenuInitBegin(menu, "") {
    Q_UNUSED(a);
    QMenu *m;
    PluginMenuInitBegin(m, tr("File")) {
      m->setIcon(HOSTICONRES("file"));
      PluginMenuAddItemIconAction(m, tr("New"), HOSTICONRES("new"),
                                  ScriptWindow::on_new);
      a->setShortcut(QKeySequence::New);
      a->setShortcutVisibleInContextMenu(true);
      PluginMenuAddItemIconAction(m, tr("Open"), HOSTICONRES("open"),
                                  ScriptWindow::on_open);
      a->setShortcut(QKeySequence::Open);
      a->setShortcutVisibleInContextMenu(true);
      PluginMenuAddItemIconAction(m, tr("Save"), HOSTICONRES("save"),
                                  ScriptWindow::on_save);
      a->setShortcut(QKeySequence::Save);
      a->setShortcutVisibleInContextMenu(true);
      PluginMenuAddItemIconAction(m, tr("SaveAs"), HOSTICONRES("saveas"),
                                  ScriptWindow::on_saveas);
      a->setShortcut(QKeySequence::SaveAs);
      a->setShortcutVisibleInContextMenu(true);
      auto rm = new QMenu(menu);
      rm->setTitle(tr("RecentFile"));
      recentmanager = new RecentFileManager(rm, this);
      recentmanager->apply();
      m->addMenu(rm);
      PluginMenuAddItemIconAction(m, tr("Close"), HOSTICONRES("closefile"),
                                  ScriptWindow::on_close);
    }
    PluginMenuInitEnd();
    menu->addMenu(m);

    PluginMenuInitBegin(m, tr("Edit")) {
      m->setIcon(HOSTICONRES("edit"));
      PluginMenuAddItemIconAction(m, tr("Undo"), HOSTICONRES("undo"),
                                  ScriptWindow::on_undo);
      a->setShortcut(QKeySequence::Undo);
      a->setShortcutVisibleInContextMenu(true);
      a->setEnabled(false);
      mundo = a;
      PluginMenuAddItemIconAction(m, tr("Redo"), HOSTICONRES("redo"),
                                  ScriptWindow::on_redo);
      a->setShortcut(QKeySequence::Redo);
      a->setShortcutVisibleInContextMenu(true);
      a->setEnabled(false);
      mredo = a;
      m->addSeparator();
      PluginMenuAddItemIconAction(m, tr("Cut"), HOSTICONRES("cut"),
                                  ScriptWindow::on_cut);
      a->setShortcut(QKeySequence::Cut);
      a->setShortcutVisibleInContextMenu(true);
      PluginMenuAddItemIconAction(m, tr("Copy"), HOSTICONRES("copy"),
                                  ScriptWindow::on_copy);
      a->setShortcut(QKeySequence::Copy);
      a->setShortcutVisibleInContextMenu(true);
      PluginMenuAddItemIconAction(m, tr("Paste"), HOSTICONRES("paste"),
                                  ScriptWindow::on_paste);
      a->setShortcut(QKeySequence::Paste);
      a->setShortcutVisibleInContextMenu(true);
      m->addSeparator();
      PluginMenuAddItemIconAction(m, tr("Find"), HOSTICONRES("find"),
                                  ScriptWindow::on_find);
      a->setShortcut(QKeySequence::Find);
      a->setShortcutVisibleInContextMenu(true);
      PluginMenuAddItemIconAction(m, tr("Replace"), ICONRES("replace"),
                                  ScriptWindow::on_replace);
      a->setShortcut(QKeySequence::Replace);
      a->setShortcutVisibleInContextMenu(true);
      PluginMenuAddItemIconAction(m, tr("Goto"), HOSTICONRES("jmp"),
                                  ScriptWindow::on_jmp);
      a->setShortcut(scgoto);
      a->setShortcutVisibleInContextMenu(true);
    }
    PluginMenuInitEnd();
    menu->addMenu(m);
    PluginMenuInitBegin(m, tr("Script")) {
      m->setIcon(ICONRES("icon"));
      PluginMenuAddItemIconAction(m, tr("Run"), ICONRES("run"),
                                  ScriptWindow::on_run);
      a->setShortcut(scrun);
      a->setShortcutVisibleInContextMenu(true);
      PluginMenuAddItemIconAction(m, tr("RunFile"), ICONRES("runf"),
                                  ScriptWindow::on_runfile);
      a->setShortcut(scrunf);
      a->setShortcutVisibleInContextMenu(true);
    }
    PluginMenuInitEnd();
    menu->addMenu(m);
    PluginMenuInitBegin(m, tr("About")) {
      m->setIcon(HOSTICONRES("author"));
      PluginMenuAddItemIconAction(m, tr("AboutPlugin"), HOSTICONRES("soft"),
                                  ScriptWindow::on_about);
      PluginMenuAddItemIconAction(m, tr("Sponsor"), HOSTICONRES("sponsor"),
                                  ScriptWindow::on_sponsor);
    }
    PluginMenuInitEnd();
    menu->addMenu(m);
  }
  PluginMenuInitEnd();

  _title->setMenu(menu);
  _title->setFullScreenButtonVisible(true);
  _title->setSwitchThemeMenuVisible(false);
  _title->setQuitMenuVisible(false);

  PluginToolBarInitBegin(toolbar, "", "") {
    toolbar->toggleViewAction()->setVisible(false);
    PluginToolBarAddAction(toolbar, HOSTICONRES("new"), ScriptWindow::on_new,
                           tr("New"));
    PluginToolBarAddAction(toolbar, HOSTICONRES("open"), ScriptWindow::on_open,
                           tr("Open"));
    PluginToolBarAddAction(toolbar, HOSTICONRES("save"), ScriptWindow::on_save,
                           tr("Save"));
    PluginToolBarAddAction(toolbar, HOSTICONRES("saveas"),
                           ScriptWindow::on_saveas, tr("SaveAs"));
    toolbar->addSeparator();
    PluginToolBarAddAction(toolbar, HOSTICONRES("undo"), ScriptWindow::on_undo,
                           tr("Undo"));
    a->setEnabled(false);
    aundo = a;
    PluginToolBarAddAction(toolbar, HOSTICONRES("redo"), ScriptWindow::on_redo,
                           tr("Redo"));
    a->setEnabled(false);
    aredo = a;
    PluginToolBarAddAction(toolbar, HOSTICONRES("cut"), ScriptWindow::on_cut,
                           tr("Cut"));
    PluginToolBarAddAction(toolbar, HOSTICONRES("copy"), ScriptWindow::on_copy,
                           tr("Copy"));
    PluginToolBarAddAction(toolbar, HOSTICONRES("paste"),
                           ScriptWindow::on_paste, tr("Paste"));
    toolbar->addSeparator();
    PluginToolBarAddAction(toolbar, HOSTICONRES("find"), ScriptWindow::on_find,
                           tr("Find"));
    PluginToolBarAddAction(toolbar, ICONRES("replace"),
                           ScriptWindow::on_replace, tr("Replace"));
    PluginToolBarAddAction(toolbar, HOSTICONRES("jmp"), ScriptWindow::on_jmp,
                           tr("Goto"));
    toolbar->addSeparator();
    PluginToolBarAddAction(toolbar, ICONRES("run"), ScriptWindow::on_run,
                           tr("Run"));
    PluginToolBarAddAction(toolbar, ICONRES("runf"), ScriptWindow::on_runfile,
                           tr("RunFile"));
    toolbar->addSeparator();
    PluginToolBarAddAction(toolbar, HOSTICONRES("soft"), ScriptWindow::on_about,
                           tr("AboutPlugin"));
    PluginToolBarAddAction(toolbar, HOSTICONRES("sponsor"),
                           ScriptWindow::on_sponsor, tr("Sponsor"));
  }
  PluginToolBarInitEnd();

  addToolBar(toolbar);

  status = new DStatusBar(this);

#define LoadPixMap(Var, Icon) Var.load(HOSTRESPIMG(Icon));

  DLabel *l;

#define AddNamedStatusLabel(Var, Content)                                      \
  Var = new DLabel(this);                                                      \
  Var->setText(Content);                                                       \
  status->addWidget(Var);

#define AddStatusLabel(Content) AddNamedStatusLabel(l, Content)

#define AddStausILable(PixMap, Icon, Label, OPixMap, OIcon)                    \
  LoadPixMap(PixMap, Icon);                                                    \
  LoadPixMap(OPixMap, OIcon);                                                  \
  Label = new DLabel(this);                                                    \
  Label->setPixmap(PixMap);                                                    \
  Label->setScaledContents(true);                                              \
  Label->setFixedSize(20, 20);                                                 \
  Label->setAlignment(Qt::AlignCenter);                                        \
  status->addPermanentWidget(Label);                                           \
  AddStatusLabel(QString(' '));

  AddStatusLabel(tr("row:"));
  l->setMinimumWidth(50);
  l->setAlignment(Qt::AlignCenter);
  AddNamedStatusLabel(lblrow, "1");

  AddStatusLabel(tr("col:"));
  l->setMinimumWidth(50);
  l->setAlignment(Qt::AlignCenter);
  AddNamedStatusLabel(lblcol, "1");

  AddStatusLabel(tr("len:"));
  l->setMinimumWidth(50);
  l->setAlignment(Qt::AlignCenter);
  AddNamedStatusLabel(lbllen, "0");

  AddStausILable(infoSaved, "saved", iSaved, infoUnsaved, "unsaved");
  iSaved->setToolTip(tr("InfoSave"));
  AddStausILable(infoWriteable, "writable", iReadWrite, infoReadonly,
                 "readonly");
  iReadWrite->setToolTip(tr("InfoReadWrite"));

  setStatusBar(status);

  connect(editor, &QCodeEditor::undoAvailable, this, [=](bool b) {
    aundo->setEnabled(b);
    mundo->setEnabled(b);
  });
  connect(editor, &QCodeEditor::redoAvailable, this, [=](bool b) {
    aredo->setEnabled(b);
    mredo->setEnabled(b);
  });
  connect(editor, &QCodeEditor::cursorPositionChanged, this, [=] {
    auto cur = editor->textCursor();
    lblrow->setText(QString::number(cur.blockNumber() + 1));
    lblcol->setText(QString::number(cur.columnNumber() + 1));
  });
  connect(editor, &QCodeEditor::selectionChanged, this, [=] {
    auto cur = editor->textCursor();
    lbllen->setText(QString::number(cur.selectionEnd() - cur.selectionStart()));
  });

  findbar = new FindBar(this);
  connect(findbar, &FindBar::findNext, this, [=](const QString &keyword) {
    if (!editor->find(keyword)) {
      auto cur = editor->textCursor();
      cur.movePosition(QTextCursor::Start);
      editor->setTextCursor(cur);
      DMessageManager::instance()->sendMessage(this, picon,
                                               tr("FindReachTheEnd"));
      editor->find(keyword);
    }
    editor->setFocus();
  });
  connect(findbar, &FindBar::findPrev, this, [=](const QString &keyword) {
    if (!editor->find(keyword, QTextDocument::FindFlag::FindBackward)) {
      auto cur = editor->textCursor();
      cur.movePosition(QTextCursor::End);
      editor->setTextCursor(cur);
      DMessageManager::instance()->sendMessage(this, picon,
                                               tr("FindReachTheStart"));
      editor->find(keyword, QTextDocument::FindFlag::FindBackward);
    }
    editor->setFocus();
  });
  connect(findbar, &FindBar::removeSearchKeyword, this, [=] {
    auto cur = editor->textCursor();
    cur.clearSelection();
    editor->setTextCursor(cur);
    editor->setFocus();
  });
  connect(findbar, &FindBar::updateSearchKeyword, this, [=](QString keyword) {
    if (!editor->find(keyword)) {
      auto cur = editor->textCursor();
      cur.movePosition(QTextCursor::Start);
      editor->setTextCursor(cur);
      editor->find(keyword);
    }
    editor->setFocus();
  });
  connect(findbar, &FindBar::sigFindbarClose, this, [=] {
    auto cur = editor->textCursor();
    cur.clearSelection();
    editor->setTextCursor(cur);
    editor->setFocus();
  });
  vlayout->addWidget(findbar);
  replacebar = new ReplaceBar(this);
  connect(replacebar, &ReplaceBar::replaceAll, this,
          [=](QString replaceText, QString withText) {
            auto cur = editor->textCursor();
            auto oldpos = cur.position();
            cur.movePosition(QTextCursor::Start);
            editor->setTextCursor(cur);
            cur.beginEditBlock();
            while (editor->find(replaceText)) {
              cur = editor->textCursor();
              cur.removeSelectedText();
              cur.insertText(withText);
            }
            cur.endEditBlock();
            cur.setPosition(oldpos);
            editor->setTextCursor(cur);
            editor->setFocus();
          });
  connect(replacebar, &ReplaceBar::replaceNext, this,
          [=](QString replaceText, QString withText) {
            auto cur = editor->textCursor();
            if (cur.selectedText().length()) {
              cur.beginEditBlock();
              cur.removeSelectedText();
              cur.insertText(withText);
              cur.endEditBlock();
            }
            if (!editor->find(replaceText)) {
              cur.movePosition(QTextCursor::Start);
              editor->setTextCursor(cur);
              DMessageManager::instance()->sendMessage(
                  this, picon, tr("ReplaceReachTheStart"));
            }
            editor->setFocus();
          });
  connect(replacebar, &ReplaceBar::replaceSkip, this, [=](QString keyword) {
    if (!editor->find(keyword)) {
      auto cur = editor->textCursor();
      cur.movePosition(QTextCursor::Start);
      editor->setTextCursor(cur);
      editor->find(keyword);
    }
    editor->setFocus();
  });
  connect(replacebar, &ReplaceBar::updateSearchKeyword, this,
          [=](QString keyword) {
            if (!editor->find(keyword)) {
              auto cur = editor->textCursor();
              cur.movePosition(QTextCursor::Start);
              editor->setTextCursor(cur);
              editor->find(keyword);
            }
            editor->setFocus();
          });
  connect(replacebar, &ReplaceBar::sigReplacebarClose, this, [=] {
    auto cur = editor->textCursor();
    cur.clearSelection();
    editor->setTextCursor(cur);
    editor->setFocus();
  });
  vlayout->addWidget(replacebar);
  connect(editor, &QCodeEditor::textChanged, this, [=] {
    if (isSaved)
      this->setSaved(false);
  });

  QShortcut *s;

#define ConnectShortCut(ShortCut, Slot)                                        \
  s = new QShortcut(ShortCut, this);                                           \
  connect(s, &QShortcut::activated, this, &Slot);

  ConnectShortCut(QKeySequence::Undo, ScriptWindow::on_undo);
  ConnectShortCut(QKeySequence::Copy, ScriptWindow::on_copy);
  ConnectShortCut(QKeySequence::Cut, ScriptWindow::on_cut);
  ConnectShortCut(QKeySequence::Paste, ScriptWindow::on_paste);
  ConnectShortCut(QKeySequence::Redo, ScriptWindow::on_redo);
  ConnectShortCut(QKeySequence::Find, ScriptWindow::on_find);
  ConnectShortCut(QKeySequence::New, ScriptWindow::on_new);
  ConnectShortCut(QKeySequence::Open, ScriptWindow::on_open);
  ConnectShortCut(QKeySequence::Save, ScriptWindow::on_save);
  ConnectShortCut(QKeySequence::SaveAs, ScriptWindow::on_saveas);
  ConnectShortCut(QKeySequence::Replace, ScriptWindow::on_replace);
  ConnectShortCut(scrun, ScriptWindow::on_run);
  ConnectShortCut(scrunf, ScriptWindow::on_runfile);
  ConnectShortCut(scgoto, ScriptWindow::on_jmp);

  QSettings settings(QApplication::organizationName(), "WingHexPy");
  lastusedpath = settings.value("curpath").toString();

  Dtk::Widget::moveToCenter(this);
}

ScriptWindow::~ScriptWindow() {
  QSettings settings(QApplication::organizationName(), "WingHexPy");
  settings.setValue("curpath", lastusedpath);
}

void ScriptWindow::setTheme(DGuiApplicationHelper::ColorType theme) {
  if (theme == DGuiApplicationHelper::LightType) {
    editor->setSyntaxStyle(m_styles[0]);
  } else {
    editor->setSyntaxStyle(m_styles[1]);
  }
}

void ScriptWindow::setSaved(bool b) {
  isSaved = b;
  iSaved->setPixmap(b ? infoSaved : infoUnsaved);
}

void ScriptWindow::on_new() {
  if (!isSaved && QMessageBox::question(this, tr("CloseConfirm"),
                                        tr("NotSaved")) == QMessageBox::No) {
    return;
  }
  editor->clear();
  currentfilename.clear();
  iReadWrite->setPixmap(infoWriteable);
}

void ScriptWindow::on_open() {
  auto filename = QFileDialog::getOpenFileName(this, tr("ChooseFile"),
                                               lastusedpath, "Python (*.py)");
  if (!filename.isEmpty()) {
    QFileInfo finfo(filename);
    lastusedpath = finfo.absoluteDir().absolutePath();
    iReadWrite->setPixmap(finfo.permission(QFile::Permission::WriteUser)
                              ? infoWriteable
                              : infoReadonly);
    QFile f(filename);
    if (f.open(QFile::ReadOnly)) {
      editor->setText(f.readAll());
      currentfilename = filename;
      f.close();
      setSaved(true);
    } else {
      DMessageManager::instance()->sendMessage(this, HOSTICONRES("open"),
                                               tr("OpenFail"));
    }
  }
}

void ScriptWindow::on_save() {
  if (currentfilename.isEmpty()) {
    on_saveas();
    return;
  }
  QFile f(currentfilename);
  if (f.open(QFile::WriteOnly)) {
    f.write(editor->toPlainText().toUtf8());
    f.close();
    setSaved(true);
  } else {
    DMessageManager::instance()->sendMessage(this, HOSTICONRES("save"),
                                             tr("SaveFail"));
  }
}

void ScriptWindow::on_saveas() {
  auto filename = QFileDialog::getSaveFileName(this, tr("ChooseSaveFile"),
                                               lastusedpath, "Python (*.py)");
  if (filename.isEmpty())
    return;
  lastusedpath = QFileInfo(filename).absoluteDir().absolutePath();
  QFile f(filename);
  if (f.open(QFile::WriteOnly)) {
    f.write(editor->toPlainText().toUtf8());
    f.close();
    setSaved(true);
    iReadWrite->setPixmap(infoWriteable);
  } else {
    DMessageManager::instance()->sendMessage(this, HOSTICONRES("saveas"),
                                             tr("SaveAsFail"));
  }
}

void ScriptWindow::on_close() { close(); }

void ScriptWindow::on_redo() { editor->redo(); }

void ScriptWindow::on_undo() { editor->undo(); }

void ScriptWindow::on_copy() { editor->copy(); }

void ScriptWindow::on_cut() { editor->cut(); }

void ScriptWindow::on_paste() { editor->paste(); }

void ScriptWindow::on_run() {
  if (!PlgInterface::instance()->RunPyText(editor->toPlainText())) {
    DMessageManager::instance()->sendMessage(this, ICONRES("run"),
                                             tr("OtherScriptRunning"));
  }
}

void ScriptWindow::on_runfile() {
  auto filename = DFileDialog::getOpenFileName(this, tr("ChoosePyScript"),
                                               lastusedpath, "Python (*.py)");
  if (filename.isEmpty())
    return;

  if (!PlgInterface::instance()->RunPyFile(filename)) {
    DMessageManager::instance()->sendMessage(this, ICONRES("runf"),
                                             tr("OtherScriptRunning"));
  }
}

void ScriptWindow::on_find() {
  replacebar->close();
  auto cur = editor->textCursor();
  findbar->activeInput(cur.selectedText(), cur.blockNumber(),
                       cur.positionInBlock(),
                       editor->verticalScrollBar()->value());
}

void ScriptWindow::on_replace() {
  findbar->close();
  auto cur = editor->textCursor();
  replacebar->activeInput(cur.selectedText(), cur.blockNumber(),
                          cur.positionInBlock(),
                          editor->verticalScrollBar()->value());
}

void ScriptWindow::on_jmp() {
  auto cur = editor->textCursor();
  auto ok = false;
  auto pos = QInputDialog::getInt(this, tr("Goto"), tr("Line"), 1, 1,
                                  cur.blockNumber(), 1, &ok);
  if (ok) {
    auto p = editor->document()->findBlockByLineNumber(pos - 1).position();
    cur.setPosition(p);
    editor->setTextCursor(cur);
  }
  editor->setFocus();
}

void ScriptWindow::on_about() {
  emit sigAbout();
  raise();
}

void ScriptWindow::on_sponsor() {
  emit sigSponsor();
  raise();
}
