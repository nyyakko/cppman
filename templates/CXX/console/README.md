# Templated C++
A C++ (and C) project template with [vcpkg](https://vcpkg.io/) support. Only tested on Windows.

  - ## requirements
    you will have to have [cmake](https://cmake.org/) installed (obviously) but you will also need to have [ninja](https://ninja-build.org/) installed, because it is the only way i know of to generate the compile instructions under windows. you also will need to have [llvm](https://github.com/llvm/llvm-project) installed on your machine, but you may also modify ``configure.py`` to use GCC if you want that. i do not recommend to use MSVC's cl.exe, as its rather difficult to get it working properly without using Visual Studio.

# Scripts usage

``setup.py``: does the wildcard substitution. deletes itself after usage.
  - **> python setup.py <repo> <language [OPTIONAL]> <language_version [OPTIONAL]>**
    - \<repo>: the name of your project.
    - \<language>: the language youre using in the project. it shall be ``C`` or ``CXX``. defaults to ``CXX``.
    - \<language_version>: the version of the standard youre using. defaults to 20.
      
  - example: ``> python setup.py my_project CXX 23``
    
``configure.py``: configures cmake to find your [vcpkg](https://vcpkg.io/) toolchain file.
  - **> python configure.py <mode [OPTIONAL]>**
    - \<mode>: the compilation mode. it shall be either ``debug`` or ``release``. defaults to ``debug``.
      
  - this script expects you to have the ``CMAKE_TOOLCHAIN_FILE`` environment variable exported before hand. follow the vcpkg tutorial to learn how to do that, or just google it up.

  - example: ``> python configure release``

``build.py``: builds the project.
  - **> python build.py**

# Usage
  - ## using as github project template
    ``gh repo create <repo> --template Console``\
    ``cd ./<repo> ; python setup.py <repo> <language> <language_version> ; python configure.py``
  - ## using as local project template
    ``cd ./<repo> ; python setup.py <repo> <language> <language_version> ; python configure.py``
