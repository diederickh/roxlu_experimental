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

# returns the value of the included file or false
def parse_include_line(line):
	line = line.strip()
	if line[0] != "#":
		return False
	
	line = line.lower()
	found = line.find("#include")
	if found != 0:
		return False
	line = line[8:].strip(" \"")
	return line

def get_local_file_include(sourceFile, localFiles, includedFile):
	for other_file in localFiles:
		if re.search(includedFile, other_file):
			source_file_dir = os.path.dirname(sourceFile)
			included_file_dir = os.path.dirname(other_file)
			if source_file_dir == included_file_dir:
				print "bname: " +os.path.basename(other_file)
				return os.path.basename(other_file)
			rel_file_dir = os.path.relpath(included_file_dir, source_file_dir)
			dirname=os.path.dirname(sourceFile) +"/"
			relpath=os.path.relpath(includedFile, dirname)
			relpath = "./" +relpath
			print "Included file dir: " +included_file_dir
			print "Source file dir: " +source_file_dir
			return relpath
	return False


# Loop through each file and check if it contains an include for another local file
for f in local_files:
	if os.path.isdir(f):
		continue
	fp = open(f,'r')
 	outdir = "output/" +os.path.dirname(f)
	if os.path.exists(outdir) == False:
		os.makedirs(outdir)
	outfile = outdir +"/" +os.path.basename(f)
	#print "Out dir: " +outdir
	#print "Out file: "+outfile
	fp_out = open(outfile, 'w')
	for line in fp:
		output_line = line
		if re.search("#include", line) :
			included_file = parse_include_line(line)
			if included_file == False:
				continue
			local_include = get_local_file_include(f, local_files, included_file)
			if local_include != False:
				print "Source File: " +f
				print "Current include: " +included_file
				print "New include: " +local_include
				print "----------------------"
				output_line = "#include \"" +local_include +"\"\n"
		fp_out.write(output_line)
	fp_out.close()	
	

