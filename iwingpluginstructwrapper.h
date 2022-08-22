#ifndef IWINGPLUGINSTRUCTWRAPPER_H
#define IWINGPLUGINSTRUCTWRAPPER_H

#include "../../WingHexExplorer/wing-hex-explorer.sourcecode/WingHexExplorer/plugin/iwingplugin.h"
#include <QObject>

class IWingPluginStructWrapper : public QObject {
  Q_OBJECT
public slots:
  // HexPosition
  HexPosition *new_HexPosition() {
    auto h = new HexPosition;
    h->line = 0;
    h->column = 0;
    h->nibbleindex = 0;
    h->lineWidth = 16; //编辑器固定列数是16
    return h;
  }
  void delete_HexPosition(HexPosition *pos) { delete pos; }
  qint64 offset(HexPosition *hexpos) { return hexpos->offset(); }
  quint64 line(HexPosition *hexpos) { return hexpos->line; }
  int column(HexPosition *hexpos) { return hexpos->column; }
  quint8 lineWidth(HexPosition *hexpos) { return hexpos->lineWidth; }
  int nibbleindex(HexPosition *hexpos) { return hexpos->nibbleindex; }

  void setLine(HexPosition *hexpos, quint64 line) { hexpos->line = line; }
  void setColumn(HexPosition *hexpos, int column) { hexpos->column = column; }
  void setNibbleindex(HexPosition *hexpos, int nibbleindex) {
    hexpos->nibbleindex = nibbleindex;
  }

  // BookMark
  BookMark *new_BookMark() { return new BookMark; }
  void delete_BookMark(BookMark *bm) { delete bm; }
  qlonglong pos(BookMark *bm) { return bm->pos; }
  QString comment(BookMark *bm) { return bm->comment; }

  void setPos(BookMark *bm, qlonglong pos) { bm->pos = pos; }
  void setComment(BookMark *bm, QString comment) { bm->comment = comment; }

  // FindResult
  int fid(FindResult *fr) { return fr->fid; }
  QList<int> indices(FindResult *fr) { return fr->indices; }

  // HexMetadataAbsoluteItem
  HexMetadataAbsoluteItem *new_HexMetadataAbsoluteItem(qint64 begin, qint64 end,
                                                       QColor foreground,
                                                       QColor background,
                                                       QString comment) {
    auto meta = new HexMetadataAbsoluteItem;
    meta->begin = begin;
    meta->end = end;
    meta->foreground = foreground;
    meta->background = background;
    meta->comment = comment;
    return meta;
  }
  void delete_HexMetadataAbsoluteItem(HexMetadataAbsoluteItem *meta) {
    delete meta;
  }

  qint64 begin(HexMetadataAbsoluteItem *meta) { return meta->begin; }
  qint64 end(HexMetadataAbsoluteItem *meta) { return meta->end; }
  QColor foreground(HexMetadataAbsoluteItem *meta) { return meta->foreground; }
  QColor background(HexMetadataAbsoluteItem *meta) { return meta->background; }
  QString comment(HexMetadataAbsoluteItem *meta) { return meta->comment; }

  // HexMetadataItem
  HexMetadataItem *new_HexMetadataItem(quint64 line, int start, int length,
                                       QColor foreground, QColor background,
                                       QString comment) {
    auto meta = new HexMetadataItem;
    meta->line = line;
    meta->start = start;
    meta->length = length;
    meta->foreground = foreground;
    meta->background = background;
    meta->comment = comment;
    return meta;
  }
  void delete_HexMetadataItem(HexMetadataItem *meta) { delete meta; }

  quint64 line(HexMetadataItem *meta) { return meta->line; }
  int start(HexMetadataItem *meta) { return meta->start; }
  int length(HexMetadataItem *meta) { return meta->length; }
  QColor foreground(HexMetadataItem *meta) { return meta->foreground; }
  QColor background(HexMetadataItem *meta) { return meta->background; }
  QString comment(HexMetadataItem *meta) { return meta->comment; }
};

#endif // IWINGPLUGINSTRUCTWRAPPER_H
