#!/usr/bin/env python
import os
import commands
import re

if not os.path.exists("./nite"):
	os.system("tar -xvjf nite-bin-macosx-v1.5.2.21.tar.bz2")
	os.system("mv NITE-* nite")
	os.system("mv nite-bin-macosx-v1.5.2.21.tar.bz2 backup/")


