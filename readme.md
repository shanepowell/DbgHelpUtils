# **DbgHelp Utilities Library**
### main branch
[![Windows VS 2022 x64](https://github.com/shanepowell/DbgHelpUtils/actions/workflows/build_vs2022x64.yml/badge.svg)](https://github.com/shanepowell/DbgHelpUtils/actions/workflows/build_vs2022x64.yml)
[![Windows VS 2022 x86](https://github.com/shanepowell/DbgHelpUtils/actions/workflows/build_vs2022x86.yml/badge.svg)](https://github.com/shanepowell/DbgHelpUtils/actions/workflows/build_vs2022x86.yml)
[![CodeQL](https://github.com/shanepowell/DbgHelpUtils/actions/workflows/codeql-analysis.yml/badge.svg)](https://github.com/shanepowell/DbgHelpUtils/actions/workflows/codeql-analysis.yml)

### develop branch
[![Windows VS 2022 x64 - develop](https://github.com/shanepowell/DbgHelpUtils/actions/workflows/build_vs2022x64.yml/badge.svg?branch=develop)](https://github.com/shanepowell/DbgHelpUtils/actions/workflows/build_vs2022x64.yml?branch=develop)
[![Windows VS 2022 x86 - develop](https://github.com/shanepowell/DbgHelpUtils/actions/workflows/build_vs2022x86.yml/badge.svg?branch=develop)](https://github.com/shanepowell/DbgHelpUtils/actions/workflows/build_vs2022x86.yml?branch=develop)

DbgHelp Utilities Library is a simple wrapper around the windows dbghelp.dll library to help break down the structures in a user mode dump file. Specifically it helps in being able to list all the heap allocated memory areas in a dump file.

The library is compiled with VS 2019 16.11.3 or the pre-release version of VS 2022 v17 preview 4.1 and using the C++ latest compiler flag.
The main library has no dependencies other than the VS c++ runtime.
The sample application MiniDumper (and the test applications) has a submodule dependency on lyra for command line support and json_struct for loading json files.

The is sample application MiniDumper, build with the DbgHelp Utilities Library can dump:
* most stream types of a user mini dump file
* stack trace dumping (exceptions, threads, UST heap allocations)
* dump NT Heap allocations (frontend, backend and LFH heaps)
* dump Segment Heap allocations (frontend, backend heaps)
* dump HPA Heap allocations
* UCRT debug heap allocations (matched to the heap allocations from any of the above heap allocations)
* UST heap allocation stack traces
* statical reports of the heap allocations
* dump file heap difference lists (heap allocation differences between two dump files of the same application at different times)
* Dump PDB symbol types at specific dump memory locations (e.g. peb)

This library could be used to create an application that automates the process of triaging dump files in a company application specific manner.  
The heap allocation features are also useful in tracking down heap allocation leak issues.

The library uses symbol files to access the internal heap structures to try to be version / platform agnostic but currently has only been tested on a limited number of platform dump files.

### dbghelp.dll and symsrv.dll
To use this library requires the use of the dbghelp.dll and symsrv.dll files.  These files are not provided and must to obtained yourself from the Debugging Tools for Windows installs (part of the Windows 10 SDK). Copy the correct versions of these files (x86 or x64) to the build where the built exe lives.  The library can use the version of dbghelp.dll that is installed into windows by default but symbol file lookup will not work so heap exploring or symbol lookup stack trace dumping will not work with the default dbghelp.dll installed in windows.
