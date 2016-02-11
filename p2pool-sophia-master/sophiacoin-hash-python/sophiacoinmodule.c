#include <Python.h>

#include "sph_sophia.h"

static void SophiaCoinHash(const char *input, int length, char *output)
{
    uint32_t hashA[16], hashB[16];
    
    sph_sophia512_context ctx_sophia[2];

    sph_sophia512_init(&ctx_sophia[0]);
    sph_sophia512 (&ctx_sophia[0], input, length); 
    sph_sophia512_close(&ctx_sophia[0], hashA); 
    
    sph_sophia512_init(&ctx_sophia[1]);
    sph_sophia512 (&ctx_sophia[1], hashA, 64); 
    sph_sophia512_close(&ctx_sophia[1], hashB); 
    
    memcpy(output, hashB, 32);
}

static PyObject *sophiacoin_gethash(PyObject *self, PyObject *args)
{
    char *output;
    PyObject *value;
#if PY_MAJOR_VERSION >= 3
    PyBytesObject *input;
#else
    PyStringObject *input;
#endif
    int length;
    if (!PyArg_ParseTuple(args, "Si", &input, &length))
        return NULL;
    Py_INCREF(input);
    output = PyMem_Malloc(32);

#if PY_MAJOR_VERSION >= 3
    SophiaCoinHash((char *)PyBytes_AsString((PyObject*) input), length, output);
#else
    SophiaCoinHash((char *)PyString_AsString((PyObject*) input), length, output);
#endif
    Py_DECREF(input);
#if PY_MAJOR_VERSION >= 3
    value = Py_BuildValue("y#", output, 32);
#else
    value = Py_BuildValue("s#", output, 32);
#endif
    PyMem_Free(output);
    return value;
}

static PyMethodDef SophiacoinMethods[] = {
    { "getHash", sophiacoin_gethash, METH_VARARGS, "Returns the sophiacoin hash" },
    { NULL, NULL, 0, NULL }
};

#if PY_MAJOR_VERSION >= 3
static struct PyModuleDef SophiacoinModule = {
    PyModuleDef_HEAD_INIT,
    "sophiacoin_hash",
    "...",
    -1,
    SophiacoinMethods
};

PyMODINIT_FUNC PyInit_sophiacoin_hash(void) {
    return PyModule_Create(&SophiacoinModule);
}

#else

PyMODINIT_FUNC initsophiacoin_hash(void) {
    (void) Py_InitModule("sophiacoin_hash", SophiacoinMethods);
}
#endif
