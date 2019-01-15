[![Build Status](https://travis-ci.org/DICL/VeloxMR.svg?branch=mapreduce)](https://travis-ci.org/DICL/VeloxMR)
[![Slack room](https://img.shields.io/badge/slack-join-pink.svg)](https://dicl.slack.com/messages/general/)
[![ZenHub](https://raw.githubusercontent.com/ZenHubIO/support/master/zenhub-badge.png)](https://zenhub.com)
[![Analytics](https://ga-beacon.appspot.com/UA-87474237-1/veloxmr)](https://github.com/DICL/VeloxMR)
---
VeloxMR {#mainpage}
========

VeloxMR is a MapReduce framework implemented in C++ on the top of VeloxDF

Key features of current VeloxMR include:
 - There's no central directory service such as HDFS NameNode. 
 - The decentralized file metadata are managed by SQLite in each data node. 
 - It is fault tolerant. (Each block is replicated in successor and decessor by default)

USAGE
=====
VeloxMR default launcher is not included in this repository, if you want to use it you can find it [here][eclipsed].

The reason not to include the launcher inside the package is to let the user to choose any launcher or service managers such as:
 - systemd/init.d
 - puppet/chef/salt
 
Once the system is up and running, you can interact with VeloxDFS with the following commands:
```
 $ dfs put|get|cat|ls|rm|format|pget|update|append
```

COMPILING & INSTALLING
======================

_Detailed information can be found in the wiki of this repository_

Compiling requirements
----------------------
 - C++14 support, this is: GCC >= 4.9, Clang >= 3.4, or ICC >= 16.0.
 - Boost library >= 1.53.
 - Sqlite3 library.
 - GNU Autotools (Autoconf, Automake, Libtool).
 - Unittest++ [optional].

For single user installation for developers
-------------------------------------------

    $ mkdir -p local_eclipse/{tmp,sandbox}                 # Create a sandbox directories
    $ cd local_eclipse                                     # enter in the directory
    $ git clone git@github.com:DICL/VeloxDFS.git           # Clone the project from github
    $ cd VeloxDFS
    $ sh autogen.sh                                        # Generate configure script 
    $ cd ../tmp                                            # Go to building folder
    $ sh ../VeloxDFS/configure --prefix=`pwd`/../sandbox # Check requirements and generate the Makefile

    # If you get a boost error go the FAQ section of the README

    ### This last command will be needed whenever you want to recompile the source
    $ make [-j#] install                                   # Compile & install add -j flag to speed up

Now edit in your **~/.bashrc** or **~/.profile**:

    export PATH="/home/*..PATH/To/eclipse/..*/sandbox/bin":$PATH
    export LIBRARY_PATH="/home/*..PATH/To/eclipse/..*/sandbox/lib"
    export C_INCLUDE_PATH="/home/*..PATH/To/eclipse/..*/sandbox/include"


Default settings for VELOXDFS 
-----------------------------

    "log" : {
      "type" : "LOG_LOCAL6"
      "name" : "ECLIPSE"
      "mask" : "DEBUG"
    },

    "cache" : {
      "numbin"      : 100,
      "size"        : 200000,
      "concurrency" : 1
    },

    "filesystem" : {
      "block"    : 137438953,
      "buffer"   : 512,
      "replica"  : 1
    }

FAQ
---

- _Question_ : `configure` stops with errors related to boost library.
- _Answer_ : It probably means that you do not have boost library installed in
  the default location, in such case you should specify the boost library location.
  ```
  sh ../VeloxDFS/configure --prefix ~/sandbox --with-boost=/usr/local --with-boost-libdir=/usr/local/lib
  ```
  In this example we assume that the boost headers are in `/usr/local/include` while the library files
  are inside `/usr/local/lib`.

AUTHORS
=======

 - __AUTHOR:__ [Vicente Adolfo Bolea Sanchez](http://vicentebolea.me)
 - __AUTHOR:__ [MooHyeon Nam](https://github.com/nammh)
 - __AUTHOR:__ [WonBae Kim](https://github.com/zwigul)
 - __AUTHOR:__ [KiBeom Jin](https://github.com/kbjin)
 - __AUTHOR:__ [Deukyeon Hwang](https://github.com/deukyeon)
 - __AUTHOR:__ [Prof. Nam Beomseok](http://dicl.unist.ac.kr)
 - __INSTITUTION:__ [DICL laboratory](http://dicl.unist.ac.kr) at _UNIST_ 
