/**************************************************************************\
 *
 *  This file is part of the Coin 3D visualization library.
 *  Copyright (C) 1998-2002 by Systems in Motion. All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public License
 *  version 2.1 as published by the Free Software Foundation. See the
 *  file LICENSE.LGPL at the root directory of the distribution for
 *  more details.
 *
 *  If you want to use Coin for applications not compatible with the
 *  LGPL, please contact SIM to acquire a Professional Edition license.
 *
 *  Systems in Motion, Prof Brochs gate 6, 7030 Trondheim, NORWAY
 *  http://www.sim.no support@sim.no Voice: +47 22114160 Fax: +47 22207097
 *
\**************************************************************************/

#ifndef COIN_SOEVENTCALLBACK_H
#define COIN_SOEVENTCALLBACK_H

#include <Inventor/nodes/SoSubNode.h>
#include <Inventor/lists/SbList.h>

#ifndef COIN_INTERNAL
// Added for Inventor compliance
#include <Inventor/actions/SoHandleEventAction.h>
#endif // !COIN_INTERNAL


class SoEventCallback;
class SoPath;
class SoEvent;
class SoPickedPoint;
class SoHandleEventAction;

typedef void SoEventCallbackCB(void * userdata, SoEventCallback * node);

#ifdef __PIVY__
%{
static void
SoEventPythonCallBack(void * userdata, SoEventCallback * node)
{
  PyObject *func, *arglist;
  PyObject *result, *evCB;

  evCB = SWIG_NewPointerObj((void *) node, SWIGTYPE_p_SoEventCallback, 1);
  /* the first item in the userdata sequence is the python callback
   * function; the second is the supplied userdata python object */
  func = PySequence_GetItem((PyObject *)userdata, 0);
  arglist = Py_BuildValue("(OO)", PySequence_GetItem((PyObject *)userdata, 1), evCB);
  result = PyEval_CallObject(func, arglist);
  Py_DECREF(arglist);
  Py_XDECREF(result);
  return /*void*/;
}
%}

%typemap(in) PyObject *pyfunc %{
  if (!PyCallable_Check($input)) {
	PyErr_SetString(PyExc_TypeError, "need a callable object!");
	return NULL;
  }
  $1 = $input;
%}
#endif

class COIN_DLL_API SoEventCallback : public SoNode {
  typedef SoNode inherited;

  SO_NODE_HEADER(SoEventCallback);

public:
  static void initClass(void);
  SoEventCallback(void);

  void setPath(SoPath * path);
  const SoPath * getPath(void);

#ifdef __PIVY__
  /* add python specific callback functions */
  %addmethods {
	void addPythonEventCallback(SoType eventtype, 
								PyObject *pyfunc, 
								PyObject *userdata = NULL) {

	  userdata = userdata != NULL ? userdata : Py_None;
	  PyObject *t = PyTuple_New(2);
	  PyTuple_SetItem(t, 0, pyfunc);
	  PyTuple_SetItem(t, 1, userdata);
	
	  self->addEventCallback(eventtype, SoEventPythonCallBack, (void *) t);
	  Py_INCREF(pyfunc);
	  Py_INCREF(userdata);
	}

	void removePythonEventCallback(SoType eventtype, 
								   PyObject *pyfunc, 
								   PyObject *userdata = NULL) {
	  userdata = userdata != NULL ? userdata : Py_None;
	  PyObject *t = PyTuple_New(2);
	  PyTuple_SetItem(t, 0, pyfunc);
	  PyTuple_SetItem(t, 1, userdata);
	
	  self->removeEventCallback(eventtype, SoEventPythonCallBack, (void *) t);
	  Py_INCREF(pyfunc);
	  Py_INCREF(userdata);
	}
  }
#endif

  void addEventCallback(SoType eventtype, SoEventCallbackCB * f,
                        void * userdata = NULL);
  void removeEventCallback(SoType eventtype, SoEventCallbackCB * f,
                           void * userdata = NULL);

  SoHandleEventAction * getAction(void) const;
  const SoEvent * getEvent(void) const;
  const SoPickedPoint * getPickedPoint(void) const;

  void setHandled(void);
  SbBool isHandled(void) const;

  void grabEvents(void);
  void releaseEvents(void);

protected:
  virtual ~SoEventCallback();

  virtual void handleEvent(SoHandleEventAction * action);

private:
  struct CallbackInfo {
    SoEventCallbackCB * func;
    SoType eventtype;
    void * userdata;

    // AIX native compiler xlC needs equality and inequality operators
    // to compile templates where these operators are referenced (even
    // if they are actually never used).

    SbBool operator==(const CallbackInfo & cbi) {
      return this->func == cbi.func && this->eventtype == cbi.eventtype && this->userdata == cbi.userdata;
    }
    SbBool operator!=(const CallbackInfo & cbi) {
      return !(*this == cbi);
    }
  };

  SbList<CallbackInfo> callbacks;
  SoHandleEventAction * heaction;
  SoPath * path;

};

#endif // !COIN_SOEVENTCALLBACK_H
