include tests/Makefile.am

AM_CPPFLAGS      = -I@srcdir@/src/common -I@srcdir@/src -include ./config.h $(BOOST_CPPFLAGS)
AM_CXXFLAGS      = $(CXXFLAGS) -Wall

bin_PROGRAMS     = eclipse_node dfs __velox_mapreduce
bin_SCRIPTS      = src/client/veloxmr src/client/veloxmr_lib.py

messages_files   = src/messages/boundaries.cc      \
                   src/messages/message.cc         \
                   src/messages/keyvalue.cc        \
                   src/messages/offsetkv.cc        \
                   src/messages/keyrequest.cc      \
                   src/messages/control.cc         \
                   src/messages/boost_impl.cc      \
                   src/messages/factory.cc         \
                   src/messages/fileinfo.cc        \
                   src/messages/fileupdate.cc      \
                   src/messages/blockinfo.cc       \
                   src/messages/blockupdate.cc     \
                   src/messages/task.cc            \
                   src/messages/reply.cc           \
                   src/messages/filerequest.cc     \
                   src/messages/blockrequest.cc    \
                   src/messages/filelist.cc        \
                   src/messages/filedescription.cc \
                   src/messages/cacheinfo.cc       \
                   src/messages/blockdel.cc        \
                   src/messages/filedel.cc         \
                   src/messages/formatrequest.cc   \
                   src/messages/fileexist.cc       \
                   src/messages/metadata.cc        \
                   src/mapreduce/messages/idatainsert.cc       \
                   src/mapreduce/messages/igroupinsert.cc      \
                   src/mapreduce/messages/iblockinsert.cc      \
                   src/mapreduce/messages/idatainforequest.cc  \
                   src/mapreduce/messages/igroupinforequest.cc \
                   src/mapreduce/messages/iblockinforequest.cc \
                   src/mapreduce/messages/idatainfo.cc         \
                   src/mapreduce/messages/idatalist.cc         \
                   src/mapreduce/messages/igroupinfo.cc        \
                   src/mapreduce/messages/iblockinfo.cc        \
                   src/mapreduce/messages/key_value_shuffle.cc \
                   src/mapreduce/messages/finish_shuffle.cc    \
                   src/messages/taskstatus.cc     \
                   src/messages/job.cc            \
                   src/messages/jobstatus.cc      \
                   src/messages/subjob.cc         \
                   src/messages/subjobstatus.cc   \
                   src/messages/idatakeys.cc      \
                   src/messages/finishmap.cc      \
                   src/messages/nodes_shuffling.cc

# libs -----
lib_LTLIBRARIES     = libvdfs.la
libvdfs_la_SOURCES  = $(messages_files) \
                      src/client/dfs.cc \
                      src/client/vdfs.cc \
                      src/common/hash.cc src/common/settings.cc \
                      src/common/logger.cc src/common/histogram.cc \
                      src/common/context.cc src/common/dl_loader.cc \
                      src/common/context_singleton.cc \
                      src/client/vmr.cc \
                      src/mapreduce/map_output_collection.cc

libvdfs_la_LDFLAGS  = $(BOOST_LDFLAGS) -version-info 0:0:0
libvdfs_la_LIBADD   = -lboost_system -lboost_serialization -lboost_coroutine -lboost_thread -lboost_context -ldl

# Binaries ----
if BOOST_STATIC
AM_LDFLAGS          = -static $(BOOST_LDFLAGS) -Wl,--start-group -Wl,-Bstatic,-lboost_system,-lboost_serialization,-lboost_coroutine,-lboost_thread,-lboost_context,-Bdynamic
LDADD               = libvdfs.la -lrt -ldl -lm
else
AM_LDFLAGS          = $(BOOST_LDFLAGS)
LDADD               = libvdfs.la -lboost_system -lboost_serialization -lboost_coroutine -lboost_thread -lboost_context -lrt -ldl -lm
endif

eclipse_node_SOURCES = src/targets/node_main_mr.cc \
                       src/network/channel.cc \
                       src/network/asyncchannel.cc \
                       src/network/p2p.cc \
                       src/network/server.cc \
                       src/network/acceptor.cc \
                       src/network/connector.cc \
                       src/nodes/machine.cc \
                       src/nodes/peerdfs.cc \
                       src/nodes/local_io.cc \
                       src/nodes/remotedfs.cc \
                       src/nodes/router.cc \
                       src/nodes/directory.cc \
                       src/nodes/node.cc \
                       src/mapreduce/nodes/peermr.cc \
                       src/mapreduce/nodes/remotemr.cc \
                       src/mapreduce/fs/directorymr.cc \
                       src/mapreduce/fs/iwriter.cc \
                       src/mapreduce/fs/ireader.cc \
                       src/mapreduce/executor.cc \
                       src/mapreduce/py_executor.cc

eclipse_node_LDADD  = $(LDADD) -lpython2.7
__velox_mapreduce_SOURCES = src/targets/veloxmr_mapreduce.cc

dfs_SOURCES         = src/client/client.cc

pkginclude_HEADERS  = src/client/vdfs.hh \
                      src/client/dfs.hh \
                      src/client/vmr.hh \
                      src/mapreduce/map_output_collection.hh

if COPY_SAMPLES
sysconf_DATA        = doc/eclipse.json
endif

dist_doc_DATA       = README.md LICENSE doc/eclipse.json
man1_MANS           = doc/eclipsefs.1

EXTRA_DIST          = doc .autotools_aux $(shell find @srcdir@/src -name "*.hh" -o -name "*.in") \
                      $(sh_scripts) doc/eclipse.json
