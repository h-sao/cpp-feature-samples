# How to compile for modules using MSVC

- Let's precompile

> cl /c /std:c++20 /EHsc hello.ixx

- Link `obj` file, and output `main.exe`
> cl /std:c++20 /EHsc /reference MyHello=MyHello.ifc main.cpp hello.obj
