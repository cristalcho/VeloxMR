SUBDIRS = VeloxDFS .

AM_CPPFLAGS      = $(CPPFLAGS) -I@srcdir@/src/common -I@srcdir@/src -I@srcdir@/VeloxDFS/src -I@srcdir@/VeloxDFS/src/common -include ./config.h $(BOOST_CPPFLAGS) -DSTANDALONE_VERSION
AM_CXXFLAGS      = $(CXXFLAGS) -Wall -g

dist_bin_SCRIPTS = src/targets/velox-compile src/targets/velox-submit
bin_PROGRAMS     = eclipse_node __velox_mapreduce
bin_SCRIPTS      = src/client/veloxmr src/client/veloxmr_lib.py
LDADD            = -lrt -ldl -lm -lvdfs

mr_messages_files= src/mapreduce/messages/task.cc            \
                   src/mapreduce/messages/boost_impl.cc \
                   src/mapreduce/messages/idatainsert.cc       \
                   src/mapreduce/messages/igroupinsert.cc      \
                   src/mapreduce/messages/iblockinsert.cc      \
                   src/mapreduce/messages/igroupinforequest.cc \
                   src/mapreduce/messages/idatainfo.cc         \
                   src/mapreduce/messages/idatalist.cc         \
                   src/mapreduce/messages/igroupinfo.cc        \
                   src/mapreduce/messages/iblockinfo.cc        \
                   src/mapreduce/messages/key_value_shuffle.cc \
                   src/mapreduce/messages/finish_shuffle.cc    \
                   src/mapreduce/messages/taskstatus.cc     \
                   src/mapreduce/messages/job.cc            \
                   src/mapreduce/messages/finishmap.cc      \
                   src/mapreduce/messages/nodes_shuffling.cc

lib_LTLIBRARIES    = libvmr.la

MEMCACHED_LDFLAGS = -L/home/hduser/libmemcached-1.0.18/build/lib
MEMCACHED_LDADD = -lmemcached -lpthread

libvmr_la_LDFLAGS  = $(BOOST_LDFLAGS) -version-info 0:0:0 $(MEMCACHED_LDFLAGS)
libvmr_la_LIBADD   = -lboost_system -lboost_serialization -lboost_coroutine -lboost_thread -lboost_context -ldl $(MEMCACHED_LDADD)
libvmr_la_SOURCES  = $(mr_messages_files) \
                      src/client/vmr.cc \
                      src/mapreduce/output_collection.cc


eclipse_node_SOURCES = src/targets/node_main_mr.cc \
                       src/mapreduce/task_executor.cc \
                       src/mapreduce/task_executor_router.cc \
                       src/mapreduce/fs/directorymr.cc \
                       src/mapreduce/fs/iwriter.cc \
                       src/mapreduce/fs/ireader.cc \
                       src/mapreduce/executor.cc \
                       src/mapreduce/task_handler.cc \
                       src/mapreduce/task_cxx.cc \
                       src/mapreduce/dl_loader.cc \
                       VeloxDFS/src/network/asyncchannel.cc \
                       VeloxDFS/src/network/server.cc \
                       VeloxDFS/src/network/client_handler.cc \
                       VeloxDFS/src/network/server_handler.cc \
                       VeloxDFS/src/network/simple_router.cc \
                       VeloxDFS/src/network/router_decorator.cc \
                       VeloxDFS/src/nodes/machine.cc \
                       VeloxDFS/src/nodes/node.cc \
                       VeloxDFS/src/blocknode/local_io.cc \
                       VeloxDFS/src/blocknode/block_node.cc \
                       VeloxDFS/src/blocknode/block_node_router.cc \
                       VeloxDFS/src/fileleader/directory.cc \
                       VeloxDFS/src/fileleader/file_leader.cc \
                       VeloxDFS/src/fileleader/file_leader_router.cc \
                       VeloxDFS/src/stats/io_monitor_invoker.cc \
                       VeloxDFS/src/stats/zk_listener.cc \
                       VeloxDFS/src/stats/scheduler_factory.cc \
                       VeloxDFS/src/stats/scheduler_score_based.cc \
                       VeloxDFS/src/stats/scheduler_simple.cc \
                       VeloxDFS/src/stats/scheduler_vlmb.cc \
                       VeloxDFS/src/stats/scheduler_python.cc \
                       VeloxDFS/src/stats/scheduler_multiwave.cc \
                       VeloxDFS/src/stats/scheduler_lean.cc
                       #src/mapreduce/task_python.cc 

eclipse_node_LDADD   = libvmr.la $(LDADD) -lpython2.7 $(libvmr_la_LIBADD) 
eclipse_node_LDADD  +=  -lzookeeper_mt

__velox_mapreduce_SOURCES = src/targets/veloxmr_mapreduce.cc
__velox_mapreduce_LDADD   = libvmr.la $(LDADD) 

pkginclude_HEADERS = src/client/vmr.hh \
                     src/mapreduce/output_collection.hh
