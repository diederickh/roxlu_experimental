@echo off

if exist build.debug (
   rd /s/q build.debug
)

if exist build.release (
   rd /s/q build.release
)

mkdir build.release
mkdir build.debug