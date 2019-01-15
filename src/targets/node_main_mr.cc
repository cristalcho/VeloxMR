#include <common/context_singleton.hh>
#include <network/server_handler.hh>
#include <network/client_handler.hh>
#include <network/simple_router.hh>
#include <fileleader/file_leader_router.hh>
#include <blocknode/block_node_router.hh>
#include <mapreduce/task_executor.hh>
#include <mapreduce/task_executor_router.hh>
#include <memory>
#include <exception>
#include <string>
#include <sys/wait.h>
#include <sys/resource.h>
#include <boost/exception/exception.hpp>
#include <unistd.h>

using namespace eclipse;

int main (int argc, char ** argv) {
  context.io.notify_fork(boost::asio::io_service::fork_prepare);

  pid_t pid = fork();
  if (pid != 0) {
    try {
      context.io.notify_fork(boost::asio::io_service::fork_parent);
      //struct rlimit limit;

      //limit.rlim_cur = 4000;
      //limit.rlim_max = 4096;
      //if (setrlimit(RLIMIT_NOFILE, &limit) != 0) {
      //  ERROR("setrlimit() failed with errno=%d\n", errno);
      //  return 1;
      //}
      //struct rlimit core_limits;
      //core_limits.rlim_cur = core_limits.rlim_max = RLIM_INFINITY;
      //setrlimit(RLIMIT_CORE, &core_limits);


      sleep(2);

      uint32_t ex_port = GET_INT("network.ports.mapreduce");
      auto internal_net = make_unique<network::ClientHandler> (ex_port);
      auto external_net = make_unique<network::ServerHandler> (ex_port);

      TaskExecutor executor(internal_net.get());

      auto router = make_unique<TaskExecutorRouter>(&executor, new SimpleRouter());

      external_net->attach(router.get());
      internal_net->attach(router.get());

      external_net->establish();

      context.run();
      context.join();

    } catch (std::exception& e) {
      ERROR("GENERAL exception at %s", e.what());

    } catch (boost::exception& e) {
      ERROR("GENERAL exception %s", diagnostic_information(e).c_str());
    }


    wait(NULL);

  } else {
    context.io.notify_fork(boost::asio::io_service::fork_child);
   
    uint32_t ex_port = GET_INT("network.ports.client");

    auto internal_net = make_unique<network::ClientHandler> (ex_port);
    auto external_net = make_unique<network::ServerHandler> (ex_port);

    FileLeader file_leader(internal_net.get());
    BlockNode block_node(internal_net.get());

    // Decorator pattern: I want FileLeader and Block node on the same network
    auto router = make_unique<FileLeaderRouter>(&file_leader, 
        new BlockNodeRouter(&block_node, new SimpleRouter()));

    external_net->attach(router.get());

    external_net->establish();

    context.run();
    context.join();
  }

  return EXIT_SUCCESS;
}
