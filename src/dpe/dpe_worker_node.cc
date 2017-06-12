#include "dpe/dpe_worker_node.h"

#include "dpe/dpe.h"

namespace dpe
{
Solver* getSolver();

WorkerTaskExecuter::WorkerTaskExecuter() : weakptr_factory_(this)
{
}

void WorkerTaskExecuter::start()
{
  getSolver()->initAsWorker();
}

void WorkerTaskExecuter::setMasterNode(RemoteNodeController* node)
{
  this->node = node;
}

void WorkerTaskExecuter::handleCompute(base::DictionaryValue* req)
{
  std::string val;
  req->GetString("task_id", &val);
  int64 taskId = atoi(val.c_str());

  base::ThreadPool::PostTask(base::ThreadPool::COMPUTE,
      FROM_HERE,
      base::Bind(&WorkerTaskExecuter::doCompute, weakptr_factory_.GetWeakPtr(), taskId));
}

void WorkerTaskExecuter::doCompute(base::WeakPtr<WorkerTaskExecuter> self, int taskId)
{
  std::string result;
  getSolver()->compute(taskId, result);
  base::ThreadPool::PostTask(base::ThreadPool::UI,
      FROM_HERE,
      base::Bind(&WorkerTaskExecuter::finishCompute, self, taskId, result));
}

void WorkerTaskExecuter::finishCompute(base::WeakPtr<WorkerTaskExecuter> self, int taskId, const std::string& result)
{
  if (auto* pThis = self.get())
  {
    pThis->finishComputeImpl(taskId, result);
  }
}

void WorkerTaskExecuter::finishComputeImpl(int taskId, const std::string& result)
{
  node->finishTask(taskId, result);
}

DPEWorkerNode::DPEWorkerNode(const std::string& myIP, const std::string& serverIP): 
  DPENodeBase(myIP, serverIP), port(kWorkerPort), weakptr_factory_(this)
{
  
}

bool DPEWorkerNode::Start(int port)
{
  this->port = port;
  zserver = new ZServer(this);
  if (!zserver->Start(myIP, port))
  {
    zserver = NULL;
    LOG(WARNING) << "Cannot start worker node.";
    LOG(WARNING) << "ip = " << myIP;
    LOG(WARNING) << "port = " << port;
    return false;
  }
  else
  {
    LOG(INFO) << "Zserver starts at: " << zserver->GetServerAddress();
  }
  taskExecuter.start();
  remoteNode = new RemoteNodeImpl(
    this,
    zserver->GetServerAddress(),
    nextConnectionId++);
  remoteNode->connectTo(base::AddressHelper::MakeZMQTCPAddress(serverIP, kServerPort));
  return true;
}
  
int DPEWorkerNode::handleConnectRequest(const std::string& address)
{
  if (remoteNode && remoteNode->getRemoteAddress() == address)
  {
    return remoteNode->getId();
  }
  return -1;
}

int DPEWorkerNode::handleDisconnectRequest(const std::string& address)
{
  LOG(INFO) << address;
  LOG(INFO) << remoteNode->getRemoteAddress();
  if (remoteNode && remoteNode->getRemoteAddress() == address)
  {
    delete remoteNode;
    remoteNode = NULL;
    base::quit_main_loop();
  }
  return 0;
}

int DPEWorkerNode::onConnectionFinished(RemoteNodeImpl* node, bool ok)
{
  if (!ok)
  {
    delete remoteNode;
    remoteNode = NULL;
    base::quit_main_loop();
  }
  else
  {
    taskExecuter.setMasterNode(
      new RemoteNodeControllerImpl(weakptr_factory_.GetWeakPtr(), node->getWeakPtr()));
  }
  return 0;
}
  
int DPEWorkerNode::handleRequest(base::DictionaryValue* req, base::DictionaryValue* reply)
{
  std::string val;

  req->GetString("action", &val);
  
  if (val == "compute")
  {
    taskExecuter.handleCompute(req);
    reply->SetString("error_code", "0");
  }
  return 0;
}

void DPEWorkerNode::removeNode(int id)
{
}

}