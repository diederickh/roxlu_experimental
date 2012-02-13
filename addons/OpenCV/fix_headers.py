#!/usr/bin/env python

import sys
import os
import re


# Create map of localfiles
local_files = []
local_dirs = []
for dirname, dirnames, filenames in os.walk('.'):
	for subdirname in dirnames:
		subdir=os.path.join(dirname, subdirname)
		subfiles=os.listdir(subdir)
		if not subdir in local_dirs and subdir != ".":
			local_dirs.append(subdir[2:])
		for subfile in subfiles:
			fullfile=os.path.join(dirname, subdirname,subfile)
			fullfile=fullfile[2:]
			local_files.append(fullfile)

	for filename in filenames:
		if not dirname in local_dirs and subdir != ".":
			local_dirs.append(dirname[2:])
		file=os.path.join(dirname, filename)
		file=file[2:]
		exts = os.path.splitext(file)
		if exts[1] == ".h":
			local_files.append(file)

outdir = "./output"
if not os.path.exists(outdir):
	os.makedirs(outdir)
	for d in local_dirs:
		od="./output/"+d
		if not os.path.exists(od):
			os.makedirs(od)

# Loop through each file and check if it contains an include for another local file
for f in local_files:
	if os.path.isdir(f):
		continue
	fp = open(f,'r')
	output=""
	create_file = False
	for line in fp:
		if re.search("#include", line) :
			for local_file in local_files:
				if re.search(local_file, line):
					relpath=os.path.relpath(f, local_file)
					print relpath
					#print line +" - " +f
			
			
			'''
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
	'''

