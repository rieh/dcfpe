#ifndef DPE_MASTER_NODE_H_
#define DPE_MASTER_NODE_H_

#include "dpe/dpe_node_base.h"
#include "dpe/compute_model.h"

namespace dpe
{
class DPEMasterNode :
    public DPENodeBase,
    public base::RefCounted<DPEMasterNode>
{
public:
  DPEMasterNode(
    const std::string& myIP, const std::string& serverIP);
  ~DPEMasterNode();

  bool Start(int port);
  
  int handleConnectRequest(const std::string& address);

  int handleDisconnectRequest(const std::string& address);

  int onConnectionFinished(RemoteNodeImpl* node, bool ok);
  
  int handleRequest(base::DictionaryValue* req, base::DictionaryValue* reply);
  
  void removeNode(int id);
private:
  MasterTaskScheduler* scheduler;
  int port;
  std::vector<RemoteNodeImpl*> remoteNodes;
    base::WeakPtrFactory<DPEMasterNode>                 weakptr_factory_;
};
}
#endif