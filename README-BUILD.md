# how to build

**linq-cpp** is configured to build using [CMake][1].

**linq-cpp** is written to be fully compliant with the C++11 standard and cross-platform. It **has no dependencies** outside of the C++11 `std` library.

Currently, [Visual Studio Express 2012 for Windows Desktop][2] is being used to build **linq-cpp**, but any compiler compliant with the C++11 standard should work.

## steps

*CMake version 2.8.10.2*

1. Clone this GitHub repository to your machine
 - In what follows, let `~` denote the directory for your local clone
2. Run CMake
3. Set "Where is the source code:" to `~/src`
4. Set "Where to build the binaries:" to `~/build`
5. Click the "Configure" button
 - If you are prompted with "Build directory does not exist, should I create it?" choose "Yes"
6. Choose the generator for the project
 - If you are using Visual Studio 2012 and are on a 64-bit Windows machine, choose "Visual Studio 11 Win64" and "Use default native compilers"
7. Click "Finish"
8. Set `CMAKE_INSTALL_PREFIX` to `~/install`
9. Click "Generate"

[1]: http://www.cmake.org/
[2]: http://www.microsoft.com/visualstudio/eng/products/visual-studio-express-for-windows-desktop#product-express-desktop
