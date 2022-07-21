#include "scriptcenterwindow.h"
#include <DTextBrowser>
#include <DTreeWidget>
#include <QHBoxLayout>

ScriptCenterWindow::ScriptCenterWindow(DMainWindow *parent) : DDialog(parent) {
  setMinimumSize(700, 500);
  setWindowTitle(tr("ScriptCenter"));
  auto w = new QWidget(this);
  auto layout = new QHBoxLayout(w);
  auto tree = new QTreeWidget(this);
  tree->setHeaderHidden(true);
  m = ScriptManager::instance();
  m->loadTreeWidget(tree);
  layout->addWidget(tree);
  auto txt = new QTextBrowser(this);
  layout->addWidget(txt);
  addContent(w);

  connect(tree, &QTreeWidget::itemSelectionChanged, this, [=] {
    auto s = tree->selectedItems();
    if (s.count()) {
      auto si = s.first();
      auto meta = si->data(0, Qt::UserRole).value<ScriptMeta>();
      if (meta.name.length()) {
        txt->setMarkdown(QString("**%1** : %2\n\n**%3** : %4\n\n"
                                 "**%5** : %6\n\n**%7** : %8\n\n"
                                 "**%9** : %10\n\n")
                             .arg(tr("Name"))
                             .arg(meta.name)
                             .arg(tr("Author"))
                             .arg(meta.author)
                             .arg(tr("License"))
                             .arg(meta.license)
                             .arg("Version")
                             .arg(meta.version)
                             .arg("Commnet")
                             .arg(meta.commnet));
      } else {
        txt->setMarkdown(
            QString("**%1** : %2\n\n").arg(tr("Catagory")).arg(si->text(0)));
      }
    }
  });
}
