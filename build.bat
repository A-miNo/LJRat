@echo off
rmdir /Q /S build
mkdir build
rmdir /Q /S lp\logs
rmdir /Q /S lp\modules\__pycache__
rmdir /Q /S lp\__pycache__
rmdir /Q /S lp\core\__pycache__

if "%~1"=="" goto blank

if %1%==Release (
    cmake -B build . -DRELEASE_BUILD=1
    cmake --build build --config %1
)

goto end

:blank
cmake -B build .
cmake --build build

:end