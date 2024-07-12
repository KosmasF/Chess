@echo off

setlocal enabledelayedexpansion

set CPP_FILES=
for %%f in (*.cpp) do (
    set CPP_FILES=!CPP_FILES! %%f
)

echo COMPILING CPP FILES: !CPP_FILES!

g++ -c !CPP_FILES!

echo CREATING LIBRARY ...

set OBJ_FILES=
for %%f in (*.o) do (
    set OBJ_FILES=!OBJ_FILES! %%f
)

ar rcs NeuralNetwok.lib !OBJ_FILES!

:: del NeuralNetwok.lib
:: ren lib.a NeuralNetwok.lib

endlocal

@echo on