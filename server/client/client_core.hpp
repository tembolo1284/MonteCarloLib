#ifndef CLIENT_CORE_HPP
#define CLIENT_CORE_HPP

#include <grpcpp/grpcpp.h>
#include "mcoptions.grpc.pb.h"
#include <memory>
#include <string>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

class McOptionsClient {
private:
    std::unique_ptr<mcoptions::McOptionsService::Stub> stub_;
    std::string server_address_;

public:
    McOptionsClient(std::shared_ptr<Channel> channel, const std::string& address)
        : stub_(mcoptions::McOptionsService::NewStub(channel)), server_address_(address) {}
    
    const std::string& get_server_address() const {
        return server_address_;
    }
    
    mcoptions::McOptionsService::Stub* get_stub() {
        return stub_.get();
    }
};

#endif
