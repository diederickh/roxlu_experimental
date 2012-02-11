#!/usr/bin/env python

import sys
import os
import re


# Create map of localfiles
local_files = []
for dirname, dirnames, filenames in os.walk('.'):
	for subdirname in dirnames:
		subdir=os.path.join(dirname, subdirname)
		subfiles=os.listdir(subdir)
		for subfile in subfiles:
			fullfile=os.path.join(dirname, subdirname,subfile)
			local_files.append(fullfile)
			#print fullfile

	for filename in filenames:
		file=os.path.join(dirname, filename)
		file=file[2:]
		exts = os.path.splitext(file)
		if exts[1] == ".h":
			local_files.append(file)

#sys.exit("test")
outdir = "./output"
if not os.path.exists(outdir):
	os.makedirs(outdir)
	os.makedirs(outdir +"/XnEE")

# Loop through each file and check if it contains an include for another local file
for f in local_files:
	fp = open(f,'r')
	output=""
	create_file = False
	for line in fp:
		if re.search("#include", line) :
			if re.search("<", line):
				found = False
				for local_file in local_files:
					if re.search(local_file, line):
						create_file = True
						output += "#include \"" +local_file +"\"\n"
						found = True
						break
				if not found:
					output += line
			else:
				output += line
		else:
			output += line	
	if create_file:
		new_file = open(outdir +"/" +f, 'w')
		new_file.write(output)
		new_file.close()
		output = ""
		print "Created " +outdir +"/" +f 

