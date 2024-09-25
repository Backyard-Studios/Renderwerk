@echo off
pushd %~dp0

python --version > NUL
if errorlevel 1 goto errorNoPython

python .\Scripts\Launch.py clean

goto:eof

:errorNoPython
echo.
echo Error: Python not installed

PAUSE
