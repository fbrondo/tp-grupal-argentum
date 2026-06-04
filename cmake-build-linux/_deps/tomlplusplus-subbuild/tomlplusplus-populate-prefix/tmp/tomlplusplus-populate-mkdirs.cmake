# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/tp-grupal-argentum/cmake-build-linux/_deps/tomlplusplus-src"
  "/home/tp-grupal-argentum/cmake-build-linux/_deps/tomlplusplus-build"
  "/home/tp-grupal-argentum/cmake-build-linux/_deps/tomlplusplus-subbuild/tomlplusplus-populate-prefix"
  "/home/tp-grupal-argentum/cmake-build-linux/_deps/tomlplusplus-subbuild/tomlplusplus-populate-prefix/tmp"
  "/home/tp-grupal-argentum/cmake-build-linux/_deps/tomlplusplus-subbuild/tomlplusplus-populate-prefix/src/tomlplusplus-populate-stamp"
  "/home/tp-grupal-argentum/cmake-build-linux/_deps/tomlplusplus-subbuild/tomlplusplus-populate-prefix/src"
  "/home/tp-grupal-argentum/cmake-build-linux/_deps/tomlplusplus-subbuild/tomlplusplus-populate-prefix/src/tomlplusplus-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/tp-grupal-argentum/cmake-build-linux/_deps/tomlplusplus-subbuild/tomlplusplus-populate-prefix/src/tomlplusplus-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/tp-grupal-argentum/cmake-build-linux/_deps/tomlplusplus-subbuild/tomlplusplus-populate-prefix/src/tomlplusplus-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
