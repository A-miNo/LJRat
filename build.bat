rmdir /Q /S build
mkdir build
rmdir /Q /S lp\logs
rmdir /Q /S lp\modules\__pycache__
rmdir /Q /S lp\__pycache__
rmdir /Q /S lp\core\__pycache__

cmake -B build . -DCMAKE_BUILD_TYPE=%1
cmake --build build 
