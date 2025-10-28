#include <grpcpp/grpcpp.h>
#include "mcoptions.grpc.pb.h"
#include "client_core.hpp"
#include "interactive_menu.hpp"
#include "cli_parser.hpp"

int main(int argc, char** argv) {
    std::string server_address = "localhost:50051";
    
    // Check if first argument is a server address (contains ':')
    int arg_offset = 1;
    if (argc > 1 && std::string(argv[1]).find(':') != std::string::npos) {
        server_address = argv[1];
        arg_offset = 2;
    }
    
    // Create client
    auto channel = grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials());
    McOptionsClient client(channel, server_address);
    
    // If no additional arguments, run interactive mode
    if (argc <= arg_offset) {
        interactive::run_interactive_mode(client);
        return 0;
    }
    
    // CLI mode - parse and execute
    cli::CliOptions opts;
    if (!cli::parse_args(argc, argv, arg_offset, opts)) {
        return 1;
    }
    
    cli::execute_cli_request(client, opts);
    
    return 0;
}
