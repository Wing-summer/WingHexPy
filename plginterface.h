#ifndef PLGINTERFACE_H
#define PLGINTERFACE_H

#include "../WingHexExplorer/wing-hex-explorer.sourcecode/WingHexExplorer/plugin/iwingplugin.h"
#include <Python.h>
#include <QObject>

#define INFOLOG(msg) "<font color=\"green\">" + msg + "</font><br />"
#define ERRLOG(msg) "<font color=\"red\">" + msg + "</font><br />"
#define WARNLOG(msg) "<font color=\"yellow\">" + msg + "</font><br />"

class PlgInterface : public QObject {
  Q_OBJECT
public:
  explicit PlgInterface(IWingPlugin *parent);

signals:
  void log(QString message);

public:
  bool init();

  bool RunPyFile(QString filename);
  bool RunPyText(QString content);

private slots:
  //--------------reader--------------

  static PyObject *currentDoc(PyObject *self, PyObject *args); // int
  static PyObject *currentDocFilename(PyObject *self,
                                      PyObject *args); // QString

  // document
  static PyObject *isReadOnly(PyObject *self, PyObject *args); // bool
  static PyObject *isKeepSize(PyObject *self, PyObject *args); // bool
  static PyObject *isLocked(PyObject *self, PyObject *args);   // bool

  static PyObject *documentLines(PyObject *self, PyObject *args); // quint64
  static PyObject *documentBytes(PyObject *self, PyObject *args); // quint64
  static PyObject *currentPos(PyObject *self, PyObject *args);    // HexPosition
  static PyObject *selectionPos(PyObject *self, PyObject *args);  // HexPosition
  static PyObject *currentRow(PyObject *self, PyObject *args);    // quint64
  static PyObject *currentColumn(PyObject *self, PyObject *args); // quint64
  static PyObject *currentOffset(PyObject *self, PyObject *args); // quint64
  static PyObject *selectlength(PyObject *self, PyObject *args);  // quint64

  static PyObject *stringVisible(PyObject *self, PyObject *args);  // bool
  static PyObject *addressVisible(PyObject *self, PyObject *args); // bool
  static PyObject *headerVisible(PyObject *self, PyObject *args);  // bool
  static PyObject *addressBase(PyObject *self, PyObject *args);    // quint64
  static PyObject *isModified(PyObject *self, PyObject *args);     // bool

  static PyObject *isEmpty(PyObject *self, PyObject *args); // bool
  static PyObject *atEnd(PyObject *self, PyObject *args);   // bool
  static PyObject *canUndo(PyObject *self, PyObject *args); // bool
  static PyObject *canRedo(PyObject *self, PyObject *args); // bool

  static PyObject *copy(PyObject *self,
                        PyObject *args); // void : bool hex = false
  static PyObject *read(PyObject *self,
                        PyObject *args); // QByteArray : qint64 offset, int len
  static PyObject *
  searchForward(PyObject *self,
                PyObject *args); // qint64 : qint64 begin, const QByteArray &ba

  static PyObject *
  searchBackward(PyObject *self,
                 PyObject *args); // qint64 : qint64 begin, const QByteArray &ba

  static PyObject *findAllBytes(PyObject *self, PyObject *args);
  // void : qlonglong begin, qlonglong end, QByteArray b,
  // QList<quint64>&results, int maxCount = -1

  // render
  static PyObject *documentLastLine(PyObject *self, PyObject *args); // quint64
  static PyObject *documentLastColumn(PyObject *self, PyObject *args); // int

  // metadata
  static PyObject *lineHasMetadata(PyObject *self,
                                   PyObject *args); // bool : quint64 line
  static PyObject *getMetadatas(
      PyObject *self,
      PyObject *args); // QList<HexMetadataAbsoluteItem> : qint64 offset
  static PyObject *
  getMetaLine(PyObject *self,
              PyObject *args); // HexLineMetadata : quint64 line

  // bookmark
  static PyObject *lineHasBookMark(PyObject *self,
                                   PyObject *args); // bool : quint64 line
  static PyObject *
  getsBookmarkPos(PyObject *self,
                  PyObject *args); // QList<qint64> : quint64 line
  static PyObject *bookMark(PyObject *self,
                            PyObject *args); // BookMark : qint64 pos
  static PyObject *bookMarkComment(PyObject *self,
                                   PyObject *args); // QString : qint64 pos
  static PyObject *
  getBookMarks(PyObject *self,
               PyObject *args); // void : QList<BookMark> &bookmarks
  static PyObject *existBookMark(PyObject *self,
                                 PyObject *args); // bool : qint64 pos

  // extension
  static PyObject *getOpenFiles(PyObject *self,
                                PyObject *args); // QList<QString>
  static PyObject *getSupportedEncodings(PyObject *self,
                                         PyObject *args); // QStringList
  static PyObject *currentEncoding(PyObject *self, PyObject *args); // QString

  //--------------controller--------------

  // document
  static PyObject *
  switchDocument(PyObject *self,
                 PyObject *args); // bool : int index, bool gui = false
  static PyObject *setLockedFile(PyObject *self,
                                 PyObject *args); // bool : bool b
  static PyObject *setKeepSize(PyObject *self,
                               PyObject *args); // bool : bool b
  static PyObject *setStringVisible(PyObject *self,
                                    PyObject *args); // void : bool b
  static PyObject *setAddressVisible(PyObject *self,
                                     PyObject *args); // void : bool b
  static PyObject *setHeaderVisible(PyObject *self,
                                    PyObject *args); // void : bool b
  static PyObject *setAddressBase(PyObject *self,
                                  PyObject *args); // void : quint64 base

  static PyObject *undo(PyObject *self, PyObject *args); // void
  static PyObject *redo(PyObject *self, PyObject *args); // void
  static PyObject *cut(PyObject *self,
                       PyObject *args); // bool : bool hex = false
  static PyObject *paste(PyObject *self,
                         PyObject *args); // void : bool hex = false
  static PyObject *
  insert(PyObject *self,
         PyObject *args); // bool : qint64 offset, const QByteArray &data
  static PyObject *
  replace(PyObject *self,
          PyObject *args); // bool : qint64 offset, const QByteArray &data
  static PyObject *remove(PyObject *self,
                          PyObject *args); // bool : qint64 offset, int len

  // cursor
  static PyObject *moveTo(PyObject *self,
                          PyObject *args); // void:
  // quint64 line, int column, int nibbleindex
  // qint64 offset
  static PyObject *
  select(PyObject *self,
         PyObject *args); // void : quint64 line, int column, int nibbleindex
  // qint64 offset, int length
  static PyObject *
  selectOffset(PyObject *self,
               PyObject *args); // void : qint64 offset, int length
  static PyObject *setInsertionMode(PyObject *self,
                                    PyObject *args); // void : bool isinsert
  static PyObject *enabledCursor(PyObject *self,
                                 PyObject *args); // void : bool b

  // metadata
  static PyObject *metadata(PyObject *self, PyObject *args);
  // bool : qint64 begin, qint64 end, const QColor &fgcolor,
  // const QColor &bgcolor,  const QString &comment
  static PyObject *removeMetadata(PyObject *self,
                                  PyObject *args); // bool : qint64 offset
  static PyObject *clearMeta(PyObject *self,
                             PyObject *args); // bool
  static PyObject *color(PyObject *self,
                         PyObject *args); // bool
  // quint64 line, int start, int length, const QColor &fgcolor,
  // const QColor &bgcolor
  static PyObject *foreground(PyObject *self,
                              PyObject *args); // bool
  // quint64 line, int start, int length, const QColor &fgcolor
  static PyObject *background(PyObject *self,
                              PyObject *args); // bool
  // quint64 line, int start, int length, const QColor &bgcolor
  static PyObject *comment(PyObject *self,
                           PyObject *args); // bool
  // quint64 line, int start, int length, const QString &comment
  static PyObject *
  applyMetas(PyObject *self,
             PyObject *args); // void : QList<HexMetadataAbsoluteItem> metas
  static PyObject *setMetaVisible(PyObject *self,
                                  PyObject *args); // bool : bool b
  static PyObject *setMetafgVisible(PyObject *self,
                                    PyObject *args); // void : bool b
  static PyObject *setMetabgVisible(PyObject *self,
                                    PyObject *args); // void : bool b
  static PyObject *setMetaCommentVisible(PyObject *self,
                                         PyObject *args); // void : bool b

  // mainwindow
  static PyObject *newFile(PyObject *self,
                           PyObject *args); // void : bool bigfile = false
  static PyObject *
  openFile(PyObject *self,
           PyObject *args); // ErrFile : QString filename, bool readonly
  static PyObject *openDriver(PyObject *self,
                              PyObject *args); // ErrFile : QString driver
  static PyObject *closeFile(PyObject *self,
                             PyObject *args); // ErrFile : int index, bool force
  static PyObject *saveFile(PyObject *self,
                            PyObject *args); // ErrFile : int index
  static PyObject *
  exportFile(PyObject *self,
             PyObject *args); // ErrFile : QString filename, int index
  static PyObject *exportFileGUI(PyObject *self,
                                 PyObject *args); // void
  static PyObject *
  saveasFile(PyObject *self,
             PyObject *args); // ErrFile : QString filename, int index
  static PyObject *saveasFileGUI(PyObject *self,
                                 PyObject *args); // void
  static PyObject *closeCurrentFile(PyObject *self,
                                    PyObject *args); // ErrFile : bool force
  static PyObject *saveCurrentFile(PyObject *self,
                                   PyObject *args); // ErrFile
  static PyObject *openFileGUI(PyObject *self,
                               PyObject *args); // void
  static PyObject *openDriverGUI(PyObject *self,
                                 PyObject *args); // void
  static PyObject *findGUI(PyObject *self,
                           PyObject *args); // void
  static PyObject *gotoGUI(PyObject *self,
                           PyObject *args); // void
  static PyObject *fillGUI(PyObject *self,
                           PyObject *args); // void
  static PyObject *fillzeroGUI(PyObject *self,
                               PyObject *args); // void
  static PyObject *fillnopGUI(PyObject *self,
                              PyObject *args); // void

  // bookmark
  static PyObject *
  addBookMark(PyObject *self,
              PyObject *args); // bool : qint64 pos, QString comment
  static PyObject *
  modBookMark(PyObject *self,
              PyObject *args); // bool : qint64 pos, QString comment
  static PyObject *
  applyBookMarks(PyObject *self,
                 PyObject *args); // void : QList<BookMark> books
  static PyObject *removeBookMark(PyObject *self,
                                  PyObject *args); // bool : qint64 pos
  static PyObject *clearBookMark(PyObject *self,
                                 PyObject *args); // bool

  // workspace
  static PyObject *
  openWorkSpace(PyObject *self,
                PyObject *args); // bool : QString filename, bool readonly
  static PyObject *
  setCurrentEncoding(PyObject *self,
                     PyObject *args); // bool : QString encoding

  // extension
  static PyObject *toast(PyObject *self,
                         PyObject *args); // void : QIcon icon, QString message

  //--------------service--------------

  static PyObject *print(PyObject *self, PyObject *args);
  static PyObject *errPrint(PyObject *self, PyObject *args);
  static PyObject *infoPrint(PyObject *self, PyObject *args);
  static PyObject *warnPrint(PyObject *self, PyObject *args);

  static PyObject *requestControl(PyObject *self,
                                  PyObject *args); // bool : int timeout = 1500
  static PyObject *hasControl(PyObject *self,
                              PyObject *args);                     // bool
  static PyObject *requestRelease(PyObject *self, PyObject *args); // bool

private:
  static PyMethodDef rEmbMethods[], cEmbMethods[], sEmbMethods[];

  static PyModuleDef WingPlgReader, WingPlgCtl, WingPlgService;
  static IWingPlugin *plg;
  static PlgInterface *self;

private:
  static PyObject *PyInit_WingPlgReader(void);
  static PyObject *PyInit_WingPlgCtl(void);
  static PyObject *PyInit_WingPlgService(void);
};

#endif // PLGINTERFACE_H
