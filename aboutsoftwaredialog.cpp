#include "aboutsoftwaredialog.h"
#include <DLabel>
#include <DTextBrowser>
#include <QPixmap>

AboutSoftwareDialog::AboutSoftwareDialog(DMainWindow *parent)
    : DDialog(parent) {
  QPixmap pic;
  pic.load(":/img/author.jpg");
  auto l = new DLabel(this);
  l->setFixedSize(100, 100);
  l->setScaledContents(true);
  l->setPixmap(pic);
  addContent(l, Qt::AlignHCenter);
  addSpacing(10);
  auto b = new DTextBrowser(this);
  b->setSearchPaths(QStringList({":", ":/img"}));
  b->setSource(QUrl("README.md"), QTextDocument::MarkdownResource);

  b->setFixedSize(800, 500);
  b->setOpenExternalLinks(true);
  addContent(b);
}
