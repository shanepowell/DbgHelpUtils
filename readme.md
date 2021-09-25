# **DbgHlp Utilities Library**

DbgHlp Utilities Library is a simple wrapper around the windows dbghelp.dll library to help break down the structures in a user mode dump file. Specifically it helps in being able to list all the heap allocated memory areas in a dump file.

The library is compiled with the pre-release version of VS 2022 and using the C++ latest compiler flag.
The main library has no dependencies other than the VS c++ runtime.

The is one sample application build with the library, which is called MiniDumper and can dump:
* dump most stream types of a user mini dump file
* stack trace dumping (exceptions, threads, UST heap allocations)
* dump NT Heap allocations (frontend, backend and LFH heaps)
* dump Segment Heap allocations (frontend, backend heaps)
* dump HPA Heap allocations
* UCRT debug heap allocations (matched to the heap allocations from any of the above heap allocations)
* UST heap allocation stack traces
* statical reports of the heap allocations
* dump file heap difference lists (heap allocation differences between two dump files of the same application at different times)

This library could be used to create an application that automates the process of triaging dump files in a company application specific manner.  
The heap allocation features are also useful in tracking down heap allocation leak issues.

### dbghelp.dll and symsrv.dll
To use this library requires the use of the dbghelp.dll and symsrv.dll files.  These files are not provided and must to obtained yourself from the Debugging Tools for Windows installs (part of the Windows 10 SDK). Copy the correct versions of these files (x86 or x64) to the build where the built exe lives.  The library can use the version of dbghelp.dll that is installed into windows by default but symbol file lookup will not work so heap exploring or symbol lookup stack trace dumping will not work with the default dbghelp.dll installed in windows.
