#include <Python.h>
#include <windows.h>
#include "libkakasi.h"

#include <stdio.h>
#include <stdlib.h>

#define DLLExport __declspec(dllexport)

static PyObject *kakasiPy_Error;

static PyObject *kakasi_argvPy(PyObject *self, PyObject *args)
{
	PyObject *list;
	int i, list_sz, ret;
	char **argv_p;

	if(!PyArg_ParseTuple(args, "O!", &PyList_Type, &list)){
		return NULL;
	}

	list_sz = PyList_Size(list);
	argv_p = (char **)calloc(list_sz + 1, sizeof(char *));
	argv_p[0] = "kakasi";
	for(i = 0; i < list_sz; i++){
		argv_p[i+1] = PyString_AsString(PyList_GetItem(list, i));
	}
	kakasi_close_kanwadict();
	ret = kakasi_getopt_argv(list_sz + 1, argv_p);
	free(argv_p);

	return Py_BuildValue("i", ret);
}

static PyObject *kakasi_doPy(PyObject *self, PyObject *args)
{
	PyObject *ret_val;
	char *input, *output;

	if(!PyArg_ParseTuple(args, "s", &input)){
		return NULL;
	}

	output = kakasi_do(input);

	ret_val = Py_BuildValue("s", output);
	kakasi_free(output);

	return ret_val;
}


static PyMethodDef kakasiPy_Methods[] = {
	{"kakasi_argvPy", kakasi_argvPy, METH_VARARGS},
	{"kakasi_doPy", kakasi_doPy, METH_VARARGS},
	{NULL, NULL}
};

DLLExport void initkakasiPy()
{
	PyObject *m, *d;

	m = Py_InitModule("kakasiPy", kakasiPy_Methods);
	d = PyModule_GetDict(m);
	kakasiPy_Error = PyErr_NewException("kakasiPy.error", NULL, NULL);
	PyDict_SetItemString(d, "error", kakasiPy_Error);
}

