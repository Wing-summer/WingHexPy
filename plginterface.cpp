#include "plginterface.h"
#include <QTemporaryFile>

#define PYMETHOD(Func)                                                         \
  { #Func, Func, METH_VARARGS, "" }

#define PyBoolRETURN(b)                                                        \
  if (b) {                                                                     \
    Py_RETURN_TRUE;                                                            \
  } else {                                                                     \
    Py_RETURN_FALSE;                                                           \
  }

PyMethodDef PlgInterface::rEmbMethods[] = {PYMETHOD(currentDoc),
                                           PYMETHOD(currentDocFilename),
                                           PYMETHOD(isReadOnly),
                                           PYMETHOD(isKeepSize),
                                           PYMETHOD(isLocked),
                                           PYMETHOD(documentLines),
                                           PYMETHOD(documentBytes),
                                           PYMETHOD(currentPos),
                                           PYMETHOD(selectionPos),
                                           PYMETHOD(currentRow),
                                           PYMETHOD(currentColumn),
                                           PYMETHOD(currentOffset),
                                           PYMETHOD(selectlength),
                                           PYMETHOD(stringVisible),
                                           PYMETHOD(addressVisible),
                                           PYMETHOD(headerVisible),
                                           PYMETHOD(addressBase),
                                           PYMETHOD(isModified),
                                           PYMETHOD(isEmpty),
                                           PYMETHOD(atEnd),
                                           PYMETHOD(canUndo),
                                           PYMETHOD(canRedo),
                                           PYMETHOD(copy),
                                           PYMETHOD(read),
                                           PYMETHOD(searchForward),
                                           PYMETHOD(searchBackward),
                                           PYMETHOD(findAllBytes),
                                           PYMETHOD(documentLastLine),
                                           PYMETHOD(documentLastColumn),
                                           PYMETHOD(lineHasMetadata),
                                           PYMETHOD(getMetadatas),
                                           PYMETHOD(getMetaLine),
                                           PYMETHOD(lineHasBookMark),
                                           PYMETHOD(getsBookmarkPos),
                                           PYMETHOD(bookMark),
                                           PYMETHOD(bookMarkComment),
                                           PYMETHOD(getBookMarks),
                                           PYMETHOD(existBookMark),
                                           PYMETHOD(getOpenFiles),
                                           PYMETHOD(getSupportedEncodings),
                                           PYMETHOD(currentEncoding),
                                           {nullptr, nullptr, 0, nullptr}};

PyMethodDef PlgInterface::cEmbMethods[] = {PYMETHOD(switchDocument),
                                           PYMETHOD(setKeepSize),
                                           PYMETHOD(setStringVisible),
                                           PYMETHOD(setAddressVisible),
                                           PYMETHOD(setHeaderVisible),
                                           PYMETHOD(setAddressBase),
                                           PYMETHOD(undo),
                                           PYMETHOD(redo),
                                           PYMETHOD(cut),
                                           PYMETHOD(paste),
                                           PYMETHOD(insert),
                                           PYMETHOD(paste),
                                           PYMETHOD(remove),
                                           PYMETHOD(moveTo),
                                           PYMETHOD(select),
                                           PYMETHOD(selectOffset),
                                           PYMETHOD(setInsertionMode),
                                           PYMETHOD(enabledCursor),
                                           PYMETHOD(metadata),
                                           PYMETHOD(removeMetadata),
                                           PYMETHOD(clearMeta),
                                           PYMETHOD(color),
                                           PYMETHOD(foreground),
                                           PYMETHOD(background),
                                           PYMETHOD(comment),
                                           PYMETHOD(applyMetas),
                                           PYMETHOD(setMetaVisible),
                                           PYMETHOD(setMetafgVisible),
                                           PYMETHOD(setMetabgVisible),
                                           PYMETHOD(setMetaCommentVisible),
                                           PYMETHOD(newFile),
                                           PYMETHOD(openFile),
                                           PYMETHOD(openDriver),
                                           PYMETHOD(closeFile),
                                           PYMETHOD(saveFile),
                                           PYMETHOD(exportFile),
                                           PYMETHOD(exportFileGUI),
                                           PYMETHOD(saveasFile),
                                           PYMETHOD(saveasFileGUI),
                                           PYMETHOD(closeCurrentFile),
                                           PYMETHOD(saveCurrentFile),
                                           PYMETHOD(openFileGUI),
                                           PYMETHOD(openDriverGUI),
                                           PYMETHOD(findGUI),
                                           PYMETHOD(gotoGUI),
                                           PYMETHOD(fillGUI),
                                           PYMETHOD(fillzeroGUI),
                                           PYMETHOD(fillnopGUI),
                                           PYMETHOD(addBookMark),
                                           PYMETHOD(modBookMark),
                                           PYMETHOD(applyBookMarks),
                                           PYMETHOD(removeBookMark),
                                           PYMETHOD(clearBookMark),
                                           PYMETHOD(openWorkSpace),
                                           PYMETHOD(setCurrentEncoding),
                                           PYMETHOD(toast),
                                           {nullptr, nullptr, 0, nullptr}};

PyMethodDef PlgInterface::sEmbMethods[] = {
    PYMETHOD(print),          PYMETHOD(errPrint),
    PYMETHOD(infoPrint),      PYMETHOD(warnPrint),
    PYMETHOD(requestControl), PYMETHOD(hasControl),
    PYMETHOD(requestRelease), {nullptr, nullptr, 0, nullptr}};

PlgInterface *PlgInterface::self = nullptr;

IWingPlugin *PlgInterface::plg = nullptr;

PyModuleDef PlgInterface::WingPlgReader = {PyModuleDef_HEAD_INIT,
                                           "WingPlgReader",
                                           nullptr,
                                           -1,
                                           rEmbMethods,
                                           nullptr,
                                           nullptr,
                                           nullptr,
                                           nullptr};

PyModuleDef PlgInterface::WingPlgCtl = {PyModuleDef_HEAD_INIT,
                                        "WingPlgCtl",
                                        nullptr,
                                        -1,
                                        cEmbMethods,
                                        nullptr,
                                        nullptr,
                                        nullptr,
                                        nullptr};

PyModuleDef PlgInterface::WingPlgService = {PyModuleDef_HEAD_INIT,
                                            "WingPlgService",
                                            nullptr,
                                            -1,
                                            sEmbMethods,
                                            nullptr,
                                            nullptr,
                                            nullptr,
                                            nullptr};

PyObject *PlgInterface::PyInit_WingPlgReader(void) {
  return PyModule_Create(&WingPlgReader);
}

PyObject *PlgInterface::PyInit_WingPlgCtl(void) {
  return PyModule_Create(&WingPlgCtl);
}

PyObject *PlgInterface::PyInit_WingPlgService(void) {
  return PyModule_Create(&WingPlgService);
}

bool PlgInterface::init() {
  PyImport_AppendInittab("WingPlgReader", &PyInit_WingPlgReader);
  PyImport_AppendInittab("WingPlgCtl", &PyInit_WingPlgCtl);
  PyImport_AppendInittab("WingPlgService", &PyInit_WingPlgService);
  Py_SetProgramName(L"WingHexPy");
  Py_Initialize();
  PySys_SetPath(QDir::tempPath().toStdWString().c_str());
  return Py_IsInitialized();
}

bool PlgInterface::RunPyFile(QString filename) {
  bool res = false;

  PyObject *pModule;
  PyObject *pModuleName;
  PyObject *pModuleFunc;

  QTemporaryFile tmpf(QDir::tempPath() + "/WSXXXXXX.py", this);
  QString tmpfilename;
  if (tmpf.open()) {
    tmpfilename = tmpf.fileName();
    QFile f(filename);
    if (f.open(QFile::ReadOnly)) {
      tmpf.write(f.readAll());
      tmpf.close();
    } else {
      return false;
    }
  } else {
    return false;
  }

  QFileInfo fi(tmpfilename);
  auto im = fi.baseName().toStdString().c_str();

  pModuleName = PyUnicode_FromString(im);
  pModule = PyImport_Import(pModuleName);
  Py_CLEAR(pModuleName);

  if (pModule) {
    pModuleFunc = PyObject_GetAttrString(pModule, "main");
    if (pModuleFunc && PyCallable_Check(pModuleFunc)) {
      PyObject *pReturnVal = PyObject_CallObject(pModuleFunc, nullptr);
      Py_CLEAR(pReturnVal);
      res = true;
    } else {
    }
    Py_CLEAR(pModuleFunc);
    Py_CLEAR(pModule);
  } else {
    return false;
  }

  if (plg->hasControl())
    plg->requestRelease();
  return res;
}

bool PlgInterface::RunPyText(QString content) {
  bool res = false;

  PyObject *pModule;
  PyObject *pModuleName;
  PyObject *pModuleFunc;

  QTemporaryFile tmpf(QDir::tempPath() + "/WSXXXXXX.py", this);
  QString tmpfilename;
  if (tmpf.open()) {
    tmpfilename = tmpf.fileName();
    tmpf.write(content.toUtf8());
    tmpf.close();
  } else {
    return false;
  }

  QFileInfo fi(tmpfilename);
  auto im = fi.baseName().toStdString().c_str();

  pModuleName = PyUnicode_FromString(im);
  pModule = PyImport_Import(pModuleName);
  Py_CLEAR(pModuleName);

  if (pModule) {
    pModuleFunc = PyObject_GetAttrString(pModule, "main");
    if (pModuleFunc && PyCallable_Check(pModuleFunc)) {
      PyObject *pReturnVal = PyObject_CallObject(pModuleFunc, nullptr);
      Py_CLEAR(pReturnVal);
      res = true;
    } else {
    }
    Py_CLEAR(pModuleFunc);
    Py_CLEAR(pModule);
  } else {
    return false;
  }
  if (plg->hasControl())
    plg->requestRelease();
  return res;
}

PlgInterface::PlgInterface(IWingPlugin *parent) : QObject(parent) {
  plg = parent;
  self = this;
}

//===================================================================

PyObject *PlgInterface::currentDoc(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  Q_UNUSED(args);
  return Py_BuildValue("i", plg->reader.currentDoc());
}

PyObject *PlgInterface::currentDocFilename(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  Q_UNUSED(args);
  return Py_BuildValue("s",
                       plg->reader.currentDocFilename().toStdString().data());
}

PyObject *PlgInterface::isReadOnly(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  Q_UNUSED(args);
  return Py_BuildValue("b", plg->reader.isReadOnly());
}

PyObject *PlgInterface::isKeepSize(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  Q_UNUSED(args);
  return Py_BuildValue("b", plg->reader.isKeepSize());
}

PyObject *PlgInterface::isLocked(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  Q_UNUSED(args);
  return Py_BuildValue("b", plg->reader.isLocked());
}

PyObject *PlgInterface::documentLines(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  Q_UNUSED(args);
  return Py_BuildValue("L", plg->reader.documentLines());
}

PyObject *PlgInterface::documentBytes(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  Q_UNUSED(args);
  return Py_BuildValue("L", plg->reader.documentBytes());
}

PyObject *PlgInterface::currentPos(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  Q_UNUSED(args);
  auto p = plg->reader.currentPos();
  return Py_BuildValue("(Liii)", p.line, p.column, p.lineWidth, p.nibbleindex);
}

PyObject *PlgInterface::selectionPos(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  Q_UNUSED(args);
  auto p = plg->reader.selectionPos();
  return Py_BuildValue("(Liii)", p.line, p.column, p.lineWidth, p.nibbleindex);
}

PyObject *PlgInterface::currentRow(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  Q_UNUSED(args);
  return Py_BuildValue("L", plg->reader.currentRow());
}

PyObject *PlgInterface::currentColumn(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  Q_UNUSED(args);
  return Py_BuildValue("L", plg->reader.currentColumn());
}

PyObject *PlgInterface::currentOffset(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  Q_UNUSED(args);
  return Py_BuildValue("L", plg->reader.currentOffset());
}

PyObject *PlgInterface::selectlength(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  Q_UNUSED(args);
  return Py_BuildValue("L", plg->reader.selectlength());
}

PyObject *PlgInterface::stringVisible(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  Q_UNUSED(args);
  return Py_BuildValue("b", plg->reader.stringVisible());
}

PyObject *PlgInterface::addressVisible(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  Q_UNUSED(args);
  return Py_BuildValue("b", plg->reader.addressVisible());
}

PyObject *PlgInterface::headerVisible(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  Q_UNUSED(args);
  return Py_BuildValue("b", plg->reader.headerVisible());
}

PyObject *PlgInterface::addressBase(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  Q_UNUSED(args);
  return Py_BuildValue("L", plg->reader.addressBase());
}

PyObject *PlgInterface::isModified(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  Q_UNUSED(args);
  return Py_BuildValue("b", plg->reader.isModified());
}

PyObject *PlgInterface::isEmpty(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  Q_UNUSED(args);
  return Py_BuildValue("b", plg->reader.isEmpty());
}

PyObject *PlgInterface::atEnd(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  Q_UNUSED(args);
  return Py_BuildValue("b", plg->reader.atEnd());
}

PyObject *PlgInterface::canUndo(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  Q_UNUSED(args);
  return Py_BuildValue("b", plg->reader.canUndo());
}

PyObject *PlgInterface::canRedo(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  Q_UNUSED(args);
  return Py_BuildValue("b", plg->reader.canRedo());
}

PyObject *PlgInterface::copy(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  bool b;
  if (!PyArg_ParseTuple(args, "b", &b)) {
    plg->reader.copy();
  } else {
    plg->reader.copy(b);
  }
  Py_RETURN_NONE;
}

PyObject *PlgInterface::read(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  qint64 offset;
  int len;
  if (!PyArg_ParseTuple(args, "Li", &offset, &len)) {
    auto ba = plg->reader.read(offset, len);
    auto pba = PyByteArray_FromStringAndSize(ba, ba.length());
    return pba;
  } else {
    return Py_BuildValue("o", PyTuple_New(0));
  }
}

PyObject *PlgInterface::searchForward(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  qint64 begin;
  QByteArray ba;
  PyObject *po;
  if (PyArg_ParseTuple(args, "Lo", &begin, &po)) {
    if (po && PyTuple_Check(po)) {
      auto len = PyTuple_Size(po);
      auto buffer = PyByteArray_AsString(po);
      ba.append(buffer, int(len));
      Py_DECREF(po);
      return Py_BuildValue("L", plg->reader.searchForward(begin, ba));
    }
  }
  return Py_BuildValue("L", -1);
}

PyObject *PlgInterface::searchBackward(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  qint64 begin;
  QByteArray ba;
  PyObject *po;
  if (PyArg_ParseTuple(args, "Lo", &begin, &po)) {
    if (po && PyByteArray_Check(po)) {
      auto len = PyByteArray_Size(po);
      auto buffer = PyByteArray_AsString(po);
      ba.append(buffer, int(len));
      Py_DECREF(po);
      return Py_BuildValue("L", plg->reader.searchBackward(begin, ba));
    }
  }
  return Py_BuildValue("L", -1);
}

PyObject *PlgInterface::findAllBytes(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  qlonglong begin, end;
  QByteArray b;
  QList<quint64> results;
  int maxCount = -1;
  PyObject *ba, *ress;

  if (PyArg_ParseTuple(args, "LLooi", &begin, &end, &ba, &ress, &maxCount)) {
    if (ba && ress && PyByteArray_Check(ba) && PyList_Check(ress)) {
      auto len = PyByteArray_Size(ba);
      auto buffer = PyByteArray_AsString(ba);
      b.append(buffer, int(len));
      plg->reader.findAllBytes(begin, end, b, results, maxCount);
      for (auto item : results) {
        PyList_Append(ress, Py_BuildValue("L", item));
      }
    }
    if (ba)
      Py_DECREF(ba);
    if (ress)
      Py_DECREF(ress);
  }

  Py_RETURN_NONE;
}

PyObject *PlgInterface::documentLastLine(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  Q_UNUSED(args);
  return Py_BuildValue("L", plg->reader.documentLastLine());
}

PyObject *PlgInterface::documentLastColumn(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  Q_UNUSED(args);
  return Py_BuildValue("i", plg->reader.documentLastColumn());
}

PyObject *PlgInterface::lineHasMetadata(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  quint64 line;
  if (PyArg_ParseTuple(args, "L", &line)) {
    PyBoolRETURN(plg->reader.lineHasMetadata(line));
  }
  Py_RETURN_FALSE;
}

PyObject *PlgInterface::getMetadatas(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  qint64 line;
  if (PyArg_ParseTuple(args, "L", &line)) {
    auto datas = plg->reader.getMetadatas(line);
    auto lists = PyList_New(datas.length());
    for (auto item : datas) {
      PyList_Append(lists, Py_BuildValue("(LLiis)", item.begin, item.end,
                                         item.foreground.rgba(),
                                         item.background.rgba(),
                                         item.comment.toStdString().data()));
    }
  }
  return PyList_New(0);
}

PyObject *PlgInterface::getMetaLine(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  quint64 line;
  if (PyArg_ParseTuple(args, "L", &line)) {
    auto datas = plg->reader.getMetaLine(line);
    auto lists = PyList_New(datas.length());
    for (auto item : datas) {
      PyList_Append(lists, Py_BuildValue("(Liiis)", item.line, item.start,
                                         item.length, item.foreground.rgba(),
                                         item.background.rgba(),
                                         item.comment.toStdString().data()));
    }
  }
  return PyList_New(0);
}

PyObject *PlgInterface::lineHasBookMark(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  quint64 line;
  if (PyArg_ParseTuple(args, "L", &line)) {
    PyBoolRETURN(plg->reader.lineHasBookMark(line));
  }
  Py_RETURN_FALSE;
}

PyObject *PlgInterface::getsBookmarkPos(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  quint64 line;
  if (PyArg_ParseTuple(args, "L", &line)) {
    auto datas = plg->reader.getsBookmarkPos(line);
    auto lists = PyList_New(datas.length());
    for (auto item : datas) {
      PyList_Append(lists, Py_BuildValue("L", item));
    }
  }
  return PyList_New(0);
}

PyObject *PlgInterface::bookMark(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  qint64 offset;
  if (PyArg_ParseTuple(args, "L", &offset)) {
    auto data = plg->reader.bookMark(offset);
    return Py_BuildValue("(Ls)", data.pos, data.comment.toStdString().data());
  }
  return PyTuple_New(0);
}

PyObject *PlgInterface::bookMarkComment(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  qint64 offset;
  if (PyArg_ParseTuple(args, "L", &offset)) {
    return PyUnicode_FromString(
        plg->reader.bookMarkComment(offset).toStdString().data());
  }
  return PyUnicode_FromString("");
}

PyObject *PlgInterface::getBookMarks(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  PyObject *bm;
  if (PyArg_ParseTuple(args, "o", &bm)) {
    if (bm && PyList_Check(bm)) {
      QList<BookMark> bs;
      plg->reader.getBookMarks(bs);
      for (auto item : bs) {
        PyList_Append(bm, Py_BuildValue("Ls", item.pos,
                                        item.comment.toStdString().data()));
      }
      Py_DECREF(bm);
    }
  }
  Py_RETURN_NONE;
}

PyObject *PlgInterface::existBookMark(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  qint64 offset;
  if (PyArg_ParseTuple(args, "L", &offset)) {
    PyBoolRETURN(plg->reader.existBookMark(offset));
  }
  Py_RETURN_FALSE;
}

PyObject *PlgInterface::getOpenFiles(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  Q_UNUSED(args);
  auto files = plg->reader.getOpenFiles();
  auto ls = PyList_New(files.length());
  for (auto item : files) {
    PyList_Append(ls, PyUnicode_FromString(item.toStdString().data()));
  }
  return ls;
}

PyObject *PlgInterface::getSupportedEncodings(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  Q_UNUSED(args);
  auto encs = plg->reader.getSupportedEncodings();
  auto ls = PyList_New(encs.length());
  for (auto item : encs) {
    PyList_Append(ls, PyUnicode_FromString(item.toStdString().data()));
  }
  return ls;
}

PyObject *PlgInterface::currentEncoding(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  Q_UNUSED(args);
  return PyUnicode_FromString(
      plg->reader.currentEncoding().toStdString().data());
}

//=========================================================================

PyObject *PlgInterface::switchDocument(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  int index = -1;
  bool gui = false;
  if (PyArg_ParseTuple(args, "ib", &index, &gui)) {
    PyBoolRETURN(plg->controller.switchDocument(index, gui));
  }
  Py_RETURN_FALSE;
}

PyObject *PlgInterface::setLockedFile(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  bool b;
  if (PyArg_ParseTuple(args, "b", &b)) {
    PyBoolRETURN(plg->controller.setLockedFile(b));
  }
  Py_RETURN_FALSE;
}

PyObject *PlgInterface::setKeepSize(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  bool b;
  if (PyArg_ParseTuple(args, "b", &b)) {
    PyBoolRETURN(plg->controller.setKeepSize(b));
  }
  return Py_False;
}

PyObject *PlgInterface::setStringVisible(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  bool b;
  if (PyArg_ParseTuple(args, "b", &b)) {
    plg->controller.setStringVisible(b);
  }
  Py_RETURN_NONE;
}

PyObject *PlgInterface::setAddressVisible(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  bool b;
  if (PyArg_ParseTuple(args, "b", &b)) {
    plg->controller.setAddressVisible(b);
  }
  Py_RETURN_NONE;
}

PyObject *PlgInterface::setHeaderVisible(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  bool b;
  if (PyArg_ParseTuple(args, "b", &b)) {
    plg->controller.setHeaderVisible(b);
  }
  Py_RETURN_NONE;
}

PyObject *PlgInterface::setAddressBase(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  quint64 base;
  if (PyArg_ParseTuple(args, "L", &base)) {
    plg->controller.setAddressBase(base);
  }
  Py_RETURN_NONE;
}

PyObject *PlgInterface::undo(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  Q_UNUSED(args);
  plg->controller.undo();
  Py_RETURN_NONE;
}

PyObject *PlgInterface::redo(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  Q_UNUSED(args);
  plg->controller.redo();
  Py_RETURN_NONE;
}

PyObject *PlgInterface::cut(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  bool b;
  if (PyArg_ParseTuple(args, "b", &b)) {
    PyBoolRETURN(plg->controller.cut(b));
  }
  PyBoolRETURN(plg->controller.cut());
}

PyObject *PlgInterface::paste(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  bool b;
  if (PyArg_ParseTuple(args, "b", &b)) {
    plg->controller.paste(b);
  } else {
    plg->controller.paste();
  }
  Py_RETURN_NONE;
}

PyObject *PlgInterface::insert(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  qint64 offset;
  PyObject *ob;
  if (PyArg_ParseTuple(args, "Lo", &offset, &ob)) {
    if (ob && PyByteArray_Check(ob)) {
      QByteArray arr(PyByteArray_AsString(ob), int(PyByteArray_Size(ob)));
      PyBoolRETURN(plg->controller.insert(offset, arr));
    }
  }
  Py_XDECREF(ob);
  Py_RETURN_FALSE;
}

PyObject *PlgInterface::replace(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  qint64 offset;
  PyObject *ob;
  if (PyArg_ParseTuple(args, "Lo", &offset, &ob)) {
    if (ob && PyByteArray_Check(ob)) {
      QByteArray arr(PyByteArray_AsString(ob), int(PyByteArray_Size(ob)));
      PyBoolRETURN(plg->controller.replace(offset, arr));
    }
  }
  Py_XDECREF(ob);
  Py_RETURN_FALSE;
}

PyObject *PlgInterface::remove(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  qint64 offset;
  int len;
  if (PyArg_ParseTuple(args, "Li", &offset, &len)) {
    PyBoolRETURN(plg->controller.remove(offset, len));
  }
  Py_RETURN_FALSE;
}

PyObject *PlgInterface::moveTo(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  auto c = PyTuple_Size(args);
  if (c == 1) {
    qint64 offset;
    if (PyArg_ParseTuple(args, "L", &offset)) {
      plg->controller.moveTo(offset);
    }
  } else {
    quint64 line;
    int column;
    int nibbleindex;
    if (PyArg_ParseTuple(args, "Lii", &line, &column, &nibbleindex)) {
      plg->controller.moveTo(line, column, nibbleindex);
    }
  }
  Py_RETURN_NONE;
}

PyObject *PlgInterface::select(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  auto c = PyTuple_Size(args);
  if (c == 2) {
    qint64 offset;
    int length;
    if (PyArg_ParseTuple(args, "Li", &offset, &length)) {
      plg->controller.select(offset, length);
    }
  } else {
    quint64 line;
    int column;
    int nibbleindex;
    if (PyArg_ParseTuple(args, "Lii", &line, &column, &nibbleindex)) {
      plg->controller.select(line, column, nibbleindex);
    }
  }

  Py_RETURN_NONE;
}

PyObject *PlgInterface::selectOffset(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  qint64 offset;
  int length;
  if (PyArg_ParseTuple(args, "Li", &offset, &length)) {
    plg->controller.selectOffset(offset, length);
  }
  Py_RETURN_NONE;
}

PyObject *PlgInterface::setInsertionMode(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  bool b;
  if (PyArg_ParseTuple(args, "b", &b)) {
    plg->controller.setInsertionMode(b);
  }
  Py_RETURN_NONE;
}

PyObject *PlgInterface::enabledCursor(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  bool b;
  if (PyArg_ParseTuple(args, "b", &b)) {
    plg->controller.enabledCursor(b);
  }
  Py_RETURN_NONE;
}

PyObject *PlgInterface::metadata(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  qint64 begin, end;
  QColor fgcolor, bgcolor;
  char *comment = nullptr;
  uint nfgcolor, nbgcolor;

  if (PyArg_ParseTuple(args, "LLiis", &begin, &end, &nfgcolor, &nbgcolor,
                       &comment)) {
    fgcolor = QColor::fromRgba(nfgcolor);
    bgcolor = QColor::fromRgba(nbgcolor);
    PyBoolRETURN(
        plg->controller.metadata(begin, end, fgcolor, bgcolor, comment));
  }
  Py_RETURN_FALSE;
}

PyObject *PlgInterface::removeMetadata(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  qint64 offset;
  if (PyArg_ParseTuple(args, "L", &offset)) {
    PyBoolRETURN(plg->controller.removeMetadata(offset));
  }
  Py_RETURN_FALSE;
}

PyObject *PlgInterface::clearMeta(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  Q_UNUSED(args);
  PyBoolRETURN(plg->controller.clearMeta());
}

PyObject *PlgInterface::color(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  quint64 line;
  int start, length;
  QColor fgcolor, bgcolor;
  uint nfgcolor, nbgcolor;
  if (PyArg_ParseTuple(args, "Liiii", &line, &start, &length, &nfgcolor,
                       &nbgcolor)) {
    fgcolor = QColor::fromRgba(nfgcolor);
    bgcolor = QColor::fromRgba(nbgcolor);
    PyBoolRETURN(plg->controller.color(line, start, length, fgcolor, bgcolor));
  }
  Py_RETURN_FALSE;
}

PyObject *PlgInterface::foreground(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  quint64 line;
  int start, length;
  QColor fgcolor;
  uint nfgcolor;
  if (PyArg_ParseTuple(args, "Liii", &line, &start, &length, &nfgcolor)) {
    fgcolor = QColor::fromRgba(nfgcolor);
    PyBoolRETURN(plg->controller.foreground(line, start, length, fgcolor));
  }
  Py_RETURN_FALSE;
}

PyObject *PlgInterface::background(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  quint64 line;
  int start, length;
  QColor bgcolor;
  uint nbgcolor;
  if (PyArg_ParseTuple(args, "Liii", &line, &start, &length, &nbgcolor)) {
    bgcolor = QColor::fromRgba(nbgcolor);
    PyBoolRETURN(plg->controller.background(line, start, length, bgcolor));
  }
  Py_RETURN_FALSE;
}

PyObject *PlgInterface::comment(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  quint64 line;
  int start, length;
  char *comment = nullptr;
  if (PyArg_ParseTuple(args, "Liis", &line, &start, &length, &comment)) {
    PyBoolRETURN(plg->controller.comment(line, start, length, comment));
  }
  Py_RETURN_FALSE;
}

PyObject *PlgInterface::applyMetas(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  PyObject *po;
  if (PyArg_ParseTuple(args, "o", &po)) {
    if (po && PyList_Check(po)) {
      auto length = PyList_Size(po);
      QList<HexMetadataAbsoluteItem> metas;
      for (long i = 0; i < length; i++) {
        qint64 begin;
        qint64 end;
        QColor foreground, background;
        char *comment = nullptr;
        uint nfgcolor, nbgcolor;
        auto item = PyList_GetItem(po, i);
        if (item && PyTuple_Check(item)) {
          if (PyArg_ParseTuple(args, "LLiis", &begin, &end, &nfgcolor,
                               &nbgcolor, &comment) &&
              comment) {
            foreground = QColor::fromRgba(nfgcolor);
            background = QColor::fromRgba(nbgcolor);
            metas.push_back(HexMetadataAbsoluteItem(begin, end, foreground,
                                                    background, comment));
          }
        }
        plg->controller.applyMetas(metas);
      }
    }
  }
  Py_RETURN_NONE;
}

PyObject *PlgInterface::setMetaVisible(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  bool b;
  if (PyArg_ParseTuple(args, "b", &b)) {
    PyBoolRETURN(plg->controller.setMetaVisible(b));
  }
  Py_RETURN_FALSE;
}

PyObject *PlgInterface::setMetafgVisible(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  bool b;
  if (PyArg_ParseTuple(args, "b", &b)) {
    plg->controller.setMetafgVisible(b);
  }
  Py_RETURN_NONE;
}

PyObject *PlgInterface::setMetabgVisible(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  bool b;
  if (PyArg_ParseTuple(args, "b", &b)) {
    plg->controller.setMetabgVisible(b);
  }
  Py_RETURN_NONE;
}

PyObject *PlgInterface::setMetaCommentVisible(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  bool b;
  if (PyArg_ParseTuple(args, "b", &b)) {
    plg->controller.setMetaCommentVisible(b);
  }
  Py_RETURN_NONE;
}

PyObject *PlgInterface::newFile(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  bool b;
  if (PyArg_ParseTuple(args, "b", &b)) {
    plg->controller.newFile(b);
  } else {
    plg->controller.newFile();
  }
  Py_RETURN_NONE;
}

PyObject *PlgInterface::openFile(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  char *filename = nullptr;
  bool b;
  auto res = ErrFile::Error;
  if (PyArg_ParseTuple(args, "sb", &filename, &b)) {
    res = plg->controller.openFile(filename, b);
  }
  return Py_BuildValue("i", res);
}

PyObject *PlgInterface::openDriver(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  char *driver = nullptr;
  auto res = ErrFile::Error;
  if (PyArg_ParseTuple(args, "s", &driver)) {
    res = plg->controller.openDriver(driver);
  }
  return Py_BuildValue("i", res);
}

PyObject *PlgInterface::closeFile(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  int index;
  bool force;
  auto res = ErrFile::Error;
  if (PyArg_ParseTuple(args, "ib", &index, &force)) {
    res = plg->controller.closeFile(index, force);
  }
  return Py_BuildValue("i", res);
}

PyObject *PlgInterface::saveFile(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  int index;
  auto res = ErrFile::Error;
  if (PyArg_ParseTuple(args, "i", &index)) {
    res = plg->controller.saveFile(index);
  }
  return Py_BuildValue("i", res);
}

PyObject *PlgInterface::exportFile(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  char *filename = nullptr;
  int index;
  auto res = ErrFile::Error;
  if (PyArg_ParseTuple(args, "si", &filename, &index)) {
    res = plg->controller.exportFile(filename, index);
  }
  return Py_BuildValue("i", res);
}

PyObject *PlgInterface::exportFileGUI(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  Q_UNUSED(args);
  plg->controller.exportFileGUI();
  Py_RETURN_NONE;
}

PyObject *PlgInterface::saveasFile(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  char *filename = nullptr;
  int index;
  auto res = ErrFile::Error;
  if (PyArg_ParseTuple(args, "si", &filename, &index)) {
    res = plg->controller.saveasFile(filename, index);
  }
  return Py_BuildValue("i", res);
}

PyObject *PlgInterface::saveasFileGUI(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  Q_UNUSED(args);
  plg->controller.saveasFileGUI();
  Py_RETURN_NONE;
}

PyObject *PlgInterface::closeCurrentFile(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  bool b;
  auto res = ErrFile::Error;
  if (PyArg_ParseTuple(args, "b", &b)) {
    res = plg->controller.closeCurrentFile(b);
  } else {
    res = plg->controller.closeCurrentFile();
  }
  return Py_BuildValue("i", res);
}

PyObject *PlgInterface::saveCurrentFile(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  Q_UNUSED(args);
  auto res = plg->controller.saveCurrentFile();
  return Py_BuildValue("i", res);
}

PyObject *PlgInterface::openFileGUI(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  Q_UNUSED(args);
  plg->controller.openFileGUI();
  Py_RETURN_NONE;
}

PyObject *PlgInterface::openDriverGUI(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  Q_UNUSED(args);
  plg->controller.openDriverGUI();
  Py_RETURN_NONE;
}

PyObject *PlgInterface::findGUI(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  Q_UNUSED(args);
  plg->controller.findGUI();
  Py_RETURN_NONE;
}

PyObject *PlgInterface::gotoGUI(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  Q_UNUSED(args);
  plg->controller.gotoGUI();
  Py_RETURN_NONE;
}

PyObject *PlgInterface::fillGUI(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  Q_UNUSED(args);
  plg->controller.fillGUI();
  Py_RETURN_NONE;
}

PyObject *PlgInterface::fillzeroGUI(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  Q_UNUSED(args);
  plg->controller.fillzeroGUI();
  Py_RETURN_NONE;
}

PyObject *PlgInterface::fillnopGUI(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  Q_UNUSED(args);
  plg->controller.fillnopGUI();
  Py_RETURN_NONE;
}

PyObject *PlgInterface::addBookMark(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  qint64 pos;
  char *message = nullptr;
  if (PyArg_ParseTuple(args, "Ls", &pos, &message) && message) {
    PyBoolRETURN(plg->controller.addBookMark(pos, message));
  }
  Py_RETURN_FALSE;
}

PyObject *PlgInterface::modBookMark(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  qint64 pos;
  char *message = nullptr;
  if (PyArg_ParseTuple(args, "Ls", &pos, &message) && message) {
    PyBoolRETURN(plg->controller.modBookMark(pos, message));
  }
  Py_RETURN_FALSE;
}

PyObject *PlgInterface::applyBookMarks(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  PyObject *po;
  if (PyArg_ParseTuple(args, "o", &po)) {
    if (po && PyList_Check(po)) {
      auto length = PyList_Size(po);
      QList<BookMark> bm;
      for (long i = 0; i < length; i++) {
        qlonglong pos;
        char *comment = nullptr;
        auto item = PyList_GetItem(po, i);
        if (item && PyTuple_Check(item)) {
          if (PyArg_ParseTuple(args, "Ls", &pos, &comment) && comment) {
            bm.push_back(BookMark{pos, comment});
          }
        }
        plg->controller.applyBookMarks(bm);
      }
    }
  }
  Py_RETURN_NONE;
}

PyObject *PlgInterface::removeBookMark(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  qint64 pos;
  if (PyArg_ParseTuple(args, "L", &pos)) {
    PyBoolRETURN(plg->controller.removeBookMark(pos));
  }
  Py_RETURN_FALSE;
}

PyObject *PlgInterface::clearBookMark(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  Q_UNUSED(args);
  PyBoolRETURN(plg->controller.clearBookMark());
}

PyObject *PlgInterface::openWorkSpace(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  char *filename = nullptr;
  bool readonly;
  if (PyArg_ParseTuple(args, "sb", &filename, &readonly) && filename) {
    PyBoolRETURN(plg->controller.openWorkSpace(filename, readonly));
  }
  Py_RETURN_FALSE;
}

PyObject *PlgInterface::setCurrentEncoding(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  char *encoding = nullptr;
  if (PyArg_ParseTuple(args, "s", &encoding) && encoding) {
    PyBoolRETURN(plg->controller.setCurrentEncoding(encoding));
  }
  Py_RETURN_FALSE;
}

PyObject *PlgInterface::toast(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  char *icon = nullptr, *message = nullptr;
  if (PyArg_ParseTuple(args, "ss", &icon, &message) && message) {
    if (icon && strlen(icon)) {
      plg->controller.toast(QIcon(icon), message);
    } else {
      plg->controller.toast(QIcon(":/img/pys.png"), message);
    }
  }
  Py_RETURN_NONE;
}
//=========================================================================

PyObject *PlgInterface::print(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  char *message = nullptr;
  if (PyArg_ParseTuple(args, "s", &message) && message)
    PlgInterface::self->log(message);

  Py_RETURN_NONE;
}

PyObject *PlgInterface::errPrint(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  char *message = nullptr;
  if (PyArg_ParseTuple(args, "s", &message) && message)
    PlgInterface::self->log(ERRLOG(QString(message)));

  Py_RETURN_NONE;
}

PyObject *PlgInterface::warnPrint(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  Q_UNUSED(args);
  char *message = nullptr;
  if (PyArg_ParseTuple(args, "s", &message) && message)
    PlgInterface::self->log(WARNLOG(QString(message)));

  Py_RETURN_NONE;
}

PyObject *PlgInterface::infoPrint(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  Q_UNUSED(args);
  char *message = nullptr;
  if (PyArg_ParseTuple(args, "s", &message) && message)
    PlgInterface::self->log(INFOLOG(QString(message)));

  Py_RETURN_NONE;
}

PyObject *PlgInterface::requestControl(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  int timeout = 1500;
  if (PyArg_Parse(args, "i", &timeout)) {
    PyBoolRETURN(plg->requestControl(timeout));
  }
  PyBoolRETURN(plg->requestControl());
}

PyObject *PlgInterface::hasControl(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  Q_UNUSED(args);
  PyBoolRETURN(plg->hasControl());
}

PyObject *PlgInterface::requestRelease(PyObject *self, PyObject *args) {
  Q_UNUSED(self);
  Q_UNUSED(args);
  PyBoolRETURN(plg->requestRelease());
}
