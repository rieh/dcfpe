#include "dpe/dpe.h"

#include <iostream>

#include <windows.h>
#include <winsock2.h>
#include <Shlobj.h>
#pragma comment(lib, "ws2_32")

#include "dpe/dpe_master_node.h"
#include "dpe/dpe_worker_node.h"

namespace dpe
{
static inline std::string get_iface_address()
{
  char hostname[128];
  char localHost[128][32]={{0}};
  struct hostent* temp;
  gethostname(hostname, 128);
  temp = gethostbyname( hostname );
  for(int i=0 ; temp->h_addr_list[i] != NULL && i < 1; ++i)
  {
    strcpy(localHost[i], inet_ntoa(*(struct in_addr *)temp->h_addr_list[i]));
  }
  return localHost[0];
}

void startNetwork()
{
  WSADATA wsaData;
  WORD sockVersion = MAKEWORD(2, 2);
  if (::WSAStartup(sockVersion, &wsaData) != 0)
  {
     std::cerr << "Cannot initialize wsa" << std::endl;
     exit(-1);
  }
}

void stopNetwork()
{
  ::WSACleanup();
}

std::string removePrefix(const std::string& s, char c)
{
  const int l = static_cast<int>(s.length());
  int i = 0;
  while (i < l && s[i] == c) ++i;
  return s.substr(i);
}

scoped_refptr<DPEMasterNode> dpeMasterNode;
scoped_refptr<DPEWorkerNode> dpeWorkerNode;
std::string type = "server";
std::string myIP;
std::string serverIP;
int port = 0;
Solver* solver;

void run()
{
  LOG(INFO) << "running";
  LOG(INFO) << "type = " << type;
  LOG(INFO) << "myIP = " << myIP;
  LOG(INFO) << "serverIP = " << serverIP;

  if (type == "server")
  {
    dpeMasterNode = new DPEMasterNode(myIP, serverIP);
    dpeMasterNode->Start(port == 0 ? kServerPort : port);
  }
  else if (type == "worker")
  {
    dpeWorkerNode = new DPEWorkerNode(myIP, serverIP);
    dpeWorkerNode->Start(port == 0 ? kWorkerPort : port);
  }
  else
  {
    LOG(ERROR) << "Unknown type";
    base::quit_main_loop();
  }
}
Solver* getSolver()
{
  return solver;
}
void start_dpe_impl(Solver* solver, int argc, char* argv[])
{
  dpe::solver = solver;
  startNetwork();

  type = "server";
  myIP = get_iface_address();
  serverIP = myIP;

  int loggingLevel = 1;
  for (int i = 1; i < argc;)
  {
    const std::string str = removePrefix(argv[i], '-');

    if (str == "t")
    {
      type = argv[i+1];
      i += 2;
    }
    else if (str == "s")
    {
      serverIP = argv[i+1];
      i += 2;
    }
    else if (str == "l") {
      loggingLevel = atoi(argv[i+1]);
      i += 2;
    }
    else if (str == "p") {
      port = atoi(argv[i+1]);
      i += 2;
    }
    else
    {
      ++i;
    }
  }

  base::dpe_base_main(run, NULL, loggingLevel);

  stopNetwork();
}
}

//extern "C" {
DPE_EXPORT void start_dpe(dpe::Solver* solver, int argc, char* argv[])
{
  dpe::start_dpe_impl(solver, argc, argv);
}
//}