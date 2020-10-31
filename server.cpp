#include <grpcpp/grpcpp.h>
#include <sstream>
#include <string>
#include "myapi.grpc.pb.h"
#include "databasemanager.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using myapi::UserData;
using myapi::RegisterUserData;
using myapi::Users;
using myapi::RegisterReply;
using myapi::Page;
using myapi::PrimaryKey;
using myapi::UserService;

// Server Implementation
class UserServiceImplementation final : public UserService::Service {
    Status Register(ServerContext* context, const RegisterUserData* registerUserData,
                    RegisterReply* reply) override {
        std::cout << "Register invoked" << std::endl;
        bool success = DatabaseManager::GetInstance().Register(registerUserData);

        reply->set_status(success);
        return Status::OK;
    }
    
    Status EnumerateRecords(ServerContext* context, const Page* page,
                            Users* users) override {
        std::cout << "EnumerateRecords invoked" << std::endl;
        bool success = DatabaseManager::GetInstance().EnumerateRecords(page, users);

        return Status::OK;
    }
    
    Status GetRecord(ServerContext* context, const PrimaryKey* key,
                    Users* users) override {
        std::cout << "GetRecord invoked" << std::endl;
        bool success = DatabaseManager::GetInstance().GetRecord(key, users);

        return Status::OK;
    }
};

void RunServer() {
    std::string server_address("0.0.0.0:9090");
    UserServiceImplementation service;

    ServerBuilder builder;
    // Listen on the given address without any authentication mechanism
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // Register "service" as the instance through which
    // communication with client takes place
    builder.RegisterService(&service);

    // Assembling the server
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on port: " << server_address << std::endl;

    server->Wait();
}

int main(int argc, char* argv[]) {
    std::string database_path = "../../test.db";
    if(argc>1)
        database_path = argv[1];
    std::cout << database_path << std::endl;
    DatabaseManager& instance =  DatabaseManager::GetInstance();
    instance.SetupConfig(database_path);
    RunServer();
    return 0;
}