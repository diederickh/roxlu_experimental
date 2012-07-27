#!/usr/bin/env python

import sys
import os
import re
import commands

# Get dylibs
local_files = []
for dirname, dirnames, filenames in os.walk('.'):
	for filename in filenames:
		file=os.path.join(dirname, filename)
		file=file[2:]
		exts = os.path.splitext(file)
		if exts[1] == ".dylib":
			local_files.append(file)

# For each dylib check what other dylibs it uses
for dylib in local_files:
	result = commands.getoutput("otool -L " +dylib)
	commands.getoutput("install_name_tool -id @executable_path/" +dylib +" " +dylib)
	for line in result.split("\n"):
		print line
		for other_dylib in local_files:
			if re.search(other_dylib, line) and dylib != other_dylib:
				print "Found other: " +other_dylib +" in  " +dylib
				old_path = line.split(" ")[0]
				commands.getoutput("install_name_tool -change " +old_path +" @executable_path/../Frameworks/OpenNI.framework/Versions/Current/Libraries/" +other_dylib +" " +dylib)
