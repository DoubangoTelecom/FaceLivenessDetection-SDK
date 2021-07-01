%include <cpointer.i>

/* File : python.i */

%apply PyObject* { void* };

%typemap(in) void* {
   // https://docs.python.org/3/c-api/bytes.html#c.PyBytes_AsString
   // char* PyBytes_AsString(PyObject *o)
   // Return a pointer to the contents of o. The pointer refers to the internal buffer of o, which consists of len(o) + 1 bytes. 
   // The last byte in the buffer is always null, regardless of whether there are any other null bytes. 
   // The data must not be modified in any way, unless the object was just created using PyBytes_FromStringAndSize(NULL, size). 
   // It must not be deallocated. If o is not a bytes object at all, PyBytes_AsString() returns NULL and raises TypeError.
   if (PyBytes_Check($input)) {
	 $1 = (void *) PyBytes_AsString($input);
   }
   else if (PyString_Check($input)) {
     $1 = (void *) PyString_AsString($input);
   }
};

%include ../swig.i


