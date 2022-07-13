#ifndef SCRIPTWINDOW_H
#define SCRIPTWINDOW_H

#include "QCodeEditor/QCodeEditor.hpp"
#include "QCodeEditor/QSyntaxStyle.hpp"
#include <DGuiApplicationHelper>
#include <DMainWindow>
#include <DStatusBar>
#include <DToolBar>

DWIDGET_USE_NAMESPACE

class ScriptWindow : public DMainWindow {
  Q_OBJECT
public:
  ScriptWindow(DMainWindow *parent = nullptr);
  static ScriptWindow *instance();

private:
  void setTheme(DGuiApplicationHelper::ColorType theme);

private:
  QCodeEditor *editor;
  static ScriptWindow *m_instance;
  QSyntaxStyle *m_styles[2];
  DToolBar *toolbar;
  DStatusBar *status;
  DMenu *menu;
};

#endif // SCRIPTWINDOW_H
