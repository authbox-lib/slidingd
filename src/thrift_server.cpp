#include "SlidingHyperService.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <syslog.h>

#include "config.h"
#include "set_manager.h"
#include "thrift_server.h"

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

class SlidingHyperServiceHandler : virtual public SlidingHyperServiceIf {
 private:
  hlld_setmgr *mgr;
 public:
  SlidingHyperServiceHandler(hlld_setmgr *mgr) {
      this->mgr = mgr;
  }

  void ping(std::string& _return) {
      _return = "pong";
  }

  void add_many(const int32_t timestamp, const std::string& key, const std::vector<std::string> & values) {
    // Your implementation goes here
    for(std::string value: values) {
    }
    printf("add_many\n");
  }

  int32_t card(const int32_t timestamp, const int32_t window, const std::vector<std::string> & keys, const std::vector<std::string> & values) {
    // Your implementation goes here
    printf("card\n");
  }

  void flush() {
    // Your implementation goes here
    printf("flush\n");
  }

  void add(const int32_t timestamp, const std::string& key, const std::string& value) {
      char *values[] = {(char*)&value[0]};
      int res = setmgr_set_keys(mgr, (char*)&key[0], values, 1);
      // set does not exist
      if (res == -1 ) {
          setmgr_create_set(mgr, (char*)&key[0], NULL);
          res = setmgr_set_keys(mgr, (char*)&key[0], values, 1);
      }
      else if (res < -1) {
          syslog(LOG_ERR, "Failure to add to key %s with value %s res: %d", (char*)&key[0], res);
      }

  }

  int32_t get(const int32_t timestamp, const int16_t window, const std::string& key) {
      uint64_t estimate = 0;
      int res = setmgr_set_size(mgr, (char*)&key[0], &estimate, window);
      if (res == -1) {
          res = setmgr_create_set(mgr, (char*)&key[0], NULL);
          return 0;
      } else if (res < -1) {
          syslog(LOG_ERR, "Failed to get set cardinality %s res %d", (char*)&key[0], res);
      }
      return estimate;
  }

  int32_t get_union(const int32_t timestamp, const int16_t window, const std::vector<std::string> & keys) {
    // Your implementation goes here
    printf("get_union\n");
  }

  int32_t get_with_element(const int32_t timestamp, const int16_t window, const std::string& key, const std::string& value) {
    // Your implementation goes here
    printf("get_with_element\n");
  }

  int32_t get_union_with_element(const int32_t timestamp, const int16_t window, const std::vector<std::string> & keys, const std::string& value) {
    // Your implementation goes here
    printf("get_union_with_element\n");
  }

};

TSimpleServer *thrift_server;


void start_thrift_server(hlld_setmgr *mgr) {
  setmgr_client_checkpoint(mgr);
  int port = 9090;
  shared_ptr<SlidingHyperServiceHandler> handler(new SlidingHyperServiceHandler(mgr));
  shared_ptr<TProcessor> processor(new SlidingHyperServiceProcessor(handler));
  shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
  shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

  thrift_server = new TSimpleServer(processor, serverTransport, transportFactory, protocolFactory);
  syslog(LOG_INFO, "Starting thrift server");
  thrift_server->serve();
  syslog(LOG_INFO, "Stopping thrift server");
}

void stop_thrift_server() {
    thrift_server->stop();
}