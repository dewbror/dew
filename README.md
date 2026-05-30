dew
===
Small header-only C libraries (C99, C++11 tested).

Headers
-------
- `dewlog.h`  — logging
- `dewargs.h` — CLI argument parsing

Build and Test
--------------
~~~
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .
ctest --output-on-failure
~~~

License
-------
MIT — see [LICENSE.txt](LICENSE.txt).
