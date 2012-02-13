Compiling OpenCV 2.3.1 for RoxluLib
-----------------------------------
- Download the sources
- Create a directory "build" inside the extracted directory
- Copy "compile.sh"
- Execute compile.sh
- When compiled, create a tmp directory and copy the "Include" subdirs
(which are opencv and opencv2)
- Copy fix_headers.py into this tmp dir
- Execute fix_headers.py
- Use the headers from the "output" directory in the addon.
