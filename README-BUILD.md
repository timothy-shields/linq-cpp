# how to build **linq-cpp**

**linq-cpp** is configured to build using [CMake][1].

**linq-cpp** is written to be 100% compliant with the C++11 standard. Currently, [Visual Studio Express 2012 for Windows Desktop][2] is being used to build **linq-cpp**, but any compiler compliant with the C++11 standard should work.

Let `~` denote the directory for your local clone of the repository.

<table>
<tr><td>Where is the source code:</td><td><pre>~/src</pre></td></tr>
<tr><td>Where to build the binaries:</td><td><pre>~/build</pre></td></tr>
</table>

<table>
<tr><th>Name</th><th>Value</th></tr>
<tr><td>CMAKE_INSTALL_PREFIX</td><td><pre>~/install</pre></td></tr>
</table>

[1]: http://www.cmake.org/
[2]: http://www.microsoft.com/visualstudio/eng/products/visual-studio-express-for-windows-desktop#product-express-desktop
