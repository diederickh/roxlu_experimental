#!/usr/bin/env python
import os
import commands
import re

if not os.path.exists("./avin/Bin/sensor"):
	os.system("cd avin/Bin && tar -xvjf SensorKinect091-Bin-MacOSX-v5.1.0.25.tar.bz2")
	os.system("cd avin/Bin && mv Sensor-* sensor")
#os.system("cp resources/avin/EngineLibMakefile avin/Platform/Linux/Build/")
#os.system("cd avin/Platform/Linux/CreateRedist && chmod 777 *")
#os.system("cd avin/Platform/Linux/CreateRedist/ && ./RedistMaker")

