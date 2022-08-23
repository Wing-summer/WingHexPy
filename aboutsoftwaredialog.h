#ifndef ABOUTSOFTWAREDIALOG_H
#define ABOUTSOFTWAREDIALOG_H

#include "../../WingHexExplorer/wing-hex-explorer.sourcecode/WingHexExplorer/plugin/iwingplugin.h"
#include <DDialog>
#include <DMainWindow>
#include <QObject>

DWIDGET_USE_NAMESPACE
class AboutSoftwareDialog : public DDialog {

  Q_OBJECT
public:
  explicit AboutSoftwareDialog(DMainWindow *parent = nullptr);
};

#endif // ABOUTSOFTWAREDIALOG_H
