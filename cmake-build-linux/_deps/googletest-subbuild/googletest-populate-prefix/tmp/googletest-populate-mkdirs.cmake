# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/tp-grupal-argentum/cmake-build-linux/_deps/googletest-src"
  "/home/tp-grupal-argentum/cmake-build-linux/_deps/googletest-build"
  "/home/tp-grupal-argentum/cmake-build-linux/_deps/googletest-subbuild/googletest-populate-prefix"
  "/home/tp-grupal-argentum/cmake-build-linux/_deps/googletest-subbuild/googletest-populate-prefix/tmp"
  "/home/tp-grupal-argentum/cmake-build-linux/_deps/googletest-subbuild/googletest-populate-prefix/src/googletest-populate-stamp"
  "/home/tp-grupal-argentum/cmake-build-linux/_deps/googletest-subbuild/googletest-populate-prefix/src"
  "/home/tp-grupal-argentum/cmake-build-linux/_deps/googletest-subbuild/googletest-populate-prefix/src/googletest-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/tp-grupal-argentum/cmake-build-linux/_deps/googletest-subbuild/googletest-populate-prefix/src/googletest-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/tp-grupal-argentum/cmake-build-linux/_deps/googletest-subbuild/googletest-populate-prefix/src/googletest-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
