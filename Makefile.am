include tests/Makefile.am

AM_CPPFLAGS      = $(CPPFLAGS) -I@srcdir@/src/common -I@srcdir@/src -include ./config.h $(BOOST_CPPFLAGS)
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
                   src/messages/blockstatus.cc     \
                   src/messages/IOoperation.cc

mr_messages_files= src/mapreduce/messages/task.cc            \
                   src/mapreduce/messages/boost_impl.cc \
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
                   src/mapreduce/messages/taskstatus.cc     \
                   src/mapreduce/messages/job.cc            \
                   src/mapreduce/messages/jobstatus.cc      \
                   src/mapreduce/messages/idatakeys.cc      \
                   src/mapreduce/messages/finishmap.cc      \
                   src/mapreduce/messages/nodes_shuffling.cc

# libs -----
lib_LTLIBRARIES     =  libvdfs.la  libvmr.la
libvdfs_la_SOURCES  = $(messages_files) \
                      src/client/dfs.cc \
                      src/client/vdfs.cc \
                      src/common/hash.cc src/common/settings.cc \
                      src/common/logger.cc src/common/histogram.cc \
                      src/common/context.cc src/common/dl_loader.cc \
                      src/common/context_singleton.cc

libvdfs_la_LDFLAGS  = $(BOOST_LDFLAGS) -version-info 0:0:0
libvdfs_la_LIBADD   = -lboost_system -lboost_serialization -lboost_coroutine -lboost_thread -lboost_context -ldl

libvmr_la_LDFLAGS  = $(BOOST_LDFLAGS) -version-info 0:0:0
libvmr_la_LIBADD   = -lboost_system -lboost_serialization -lboost_coroutine -lboost_thread -lboost_context -ldl
libvmr_la_SOURCES  = $(mr_messages_files) \
                      src/client/vmr.cc \
                      src/mapreduce/output_collection.cc

# Binaries ----
if BOOST_STATIC
AM_LDFLAGS          = -static $(BOOST_LDFLAGS) -Wl,--start-group -Wl,-Bstatic,-lboost_system,-lboost_serialization,-lboost_coroutine,-lboost_thread,-lboost_context,-Bdynamic
LDADD               = libvdfs.la -lrt -ldl -lm
else
AM_LDFLAGS          = $(BOOST_LDFLAGS)
LDADD               = libvdfs.la -lboost_system -lboost_serialization -lboost_coroutine -lboost_thread -lboost_context -lrt -ldl -lm
endif

eclipse_node_SOURCES = src/targets/node_main_mr.cc \
                       src/network/asyncchannel.cc \
                       src/network/server.cc \
                       src/network/client_handler.cc \
                       src/network/server_handler.cc \
                       src/network/simple_router.cc \
                       src/network/router_decorator.cc \
                       src/nodes/machine.cc \
                       src/nodes/node.cc \
                       src/blocknode/local_io.cc \
                       src/blocknode/block_node.cc \
                       src/blocknode/block_node_router.cc \
                       src/fileleader/directory.cc \
                       src/fileleader/file_leader.cc \
                       src/fileleader/file_leader_router.cc \
                       src/mapreduce/task_executor.cc \
                       src/mapreduce/task_executor_router.cc \
                       src/mapreduce/fs/directorymr.cc \
                       src/mapreduce/fs/iwriter.cc \
                       src/mapreduce/fs/ireader.cc \
                       src/mapreduce/executor.cc \
                       src/mapreduce/task_handler.cc \
                       src/mapreduce/task_cxx.cc \
                       src/mapreduce/task_python.cc

eclipse_node_LDADD  = libvmr.la $(LDADD) -lpython2.7

dfs_SOURCES         = src/targets/client.cc \
                      src/client/cli_driver.cc

__velox_mapreduce_SOURCES = src/targets/veloxmr_mapreduce.cc
__velox_mapreduce_LDADD = libvmr.la $(LDADD) libvdfs.la

pkginclude_HEADERS  = src/client/vdfs.hh \
                      src/client/dfs.hh \
                      src/client/vmr.hh \
                      src/mapreduce/output_collection.hh

modelincludedir     = $(pkgincludedir)/model
modelinclude_HEADERS = src/client/model/metadata.hh

if COPY_SAMPLES
sysconf_DATA        = doc/eclipse.json
endif

dist_doc_DATA       = README.md LICENSE doc/eclipse.json
man1_MANS           = doc/eclipsefs.1

EXTRA_DIST          = doc .autotools_aux $(shell find @srcdir@/src -name "*.hh" -o -name "*.in") \
                      $(sh_scripts) doc/eclipse.json
