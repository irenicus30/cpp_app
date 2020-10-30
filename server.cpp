#include <grpcpp/grpcpp.h>
#include <string>
#include <sqlite3.h> 
#include "myapi.grpc.pb.h"

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

class DatabaseManager {
public:
  static DatabaseManager& getInstance() {
    static DatabaseManager instance;
    return instance;
  }

  bool connect(std::string filename) {
    int rc = sqlite3_open_v2(filename.c_str(), &db, SQLITE_OPEN_FULLMUTEX, NULL);
    if (rc != SQLITE_OK) {
      std::cout << std::endl << "Failed to initialize sqlite connection handle." << std::endl;
      return false;
    }
  }


  DatabaseManager(const DatabaseManager &) = delete;
  DatabaseManager & operator = (const DatabaseManager &) = delete;

private:
  DatabaseManager() {}
  ~DatabaseManager() {
    if(db != nullptr) {
      sqlite3_close(db);
    }
  }

  sqlite3* db = nullptr; 
};



// Server Implementation
class UserServiceImplementation final : public UserService::Service {
  Status Register(ServerContext* context, const RegisterUserData* registerUserData,
                  RegisterReply* reply) override {
    
    std::cout << "Register" << std::endl;

    reply->set_status(true);
    return Status::OK;
  }
  
  Status EnumerateRecords(ServerContext* context, const Page* page,
                          Users* users) override {
    
    std::cout << "EnumerateRecords" << std::endl;

    UserData *u1 = users->add_entry();
    u1->set_email("email from database list 1");
    u1->set_nickname("nickname from database list 1");
    u1->set_is_active(false);
    
    UserData *u2 = users->add_entry();
    u2->set_email("email from database list 2");
    u2->set_nickname("nickname from database list 2");
    u2->set_is_active(true);

    return Status::OK;
  }
  
  Status GetRecord(ServerContext* context, const PrimaryKey* key,
                   UserData* user) override {
    
    std::cout << "GetRecord" << std::endl;

    user->set_email("email from database");
    user->set_nickname("nickname from database");
    user->set_is_active(true);

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

int main(int argc, char** argv) {
  RunServer();
  return 0;
}