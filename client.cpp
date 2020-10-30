#include <grpcpp/grpcpp.h>
#include <string>
#include "myapi.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using myapi::UserData;
using myapi::RegisterUserData;
using myapi::Users;
using myapi::RegisterReply;
using myapi::Page;
using myapi::PrimaryKey;
using myapi::UserService;

class UserServiceClient {
 public:
  UserServiceClient(std::shared_ptr<Channel> channel)
      : stub_(UserService::NewStub(channel)) {}

  // Assembles client payload, sends it to the server, and returns its response
  void Register() {
    // Data to be sent to server
    RegisterUserData registerUserData;

    registerUserData.set_email("dummy email");
    registerUserData.set_nickname("dummy nickname");
    registerUserData.set_is_active(true);
    registerUserData.set_password("secret");

    // Container for server response
    RegisterReply reply;

    // Context can be used to send meta data to server or modify RPC behaviour
    ClientContext context;

    // Actual Remote Procedure Call
    Status status = stub_->Register(&context, registerUserData, &reply);

    // Returns results based on RPC status
    if (status.ok()) {
      if (reply.status()) {
        std::cout << "user registered" << std::endl;
      } else {
        std::cout << "user not registered" << std::endl;
      }
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      std::cout << "RPC Failed" << std::endl;
    }
  }
  
  void EnumerateRecords(int page_number, int page_size) {
    // Data to be sent to server
    Page page;
    page.set_page_number(page_number);
    page.set_page_size(page_size);

    // Container for server response
    Users users;

    // Context can be used to send meta data to server or modify RPC behaviour
    ClientContext context;

    // Actual Remote Procedure Call
    Status status = stub_->EnumerateRecords(&context, page, &users);

    // Returns results based on RPC status
    if (status.ok()) {
      std::cout << "EnumerateRecords ok" << std::endl;
      std::cout << "users.entry_size() " << users.entry_size() << std::endl;
      for(int i = 0; i < users.entry_size(); i++) {
        UserData user = users.entry(i);
        std::cout << user.email() << " " << user.nickname() << " " << user.is_active() << std::endl;
      }
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      std::cout << "RPC Failed" << std::endl;
    }
  }
  
  void GetRecord(std::string email) {
    // Data to be sent to server
    PrimaryKey key;
    key.set_email(email);

    // Container for server response
    Users users;

    // Context can be used to send meta data to server or modify RPC behaviour
    ClientContext context;

    // Actual Remote Procedure Call
    Status status = stub_->GetRecord(&context, key, &users);

    // Returns results based on RPC status
    if (status.ok()) {
      std::cout << "GetRecord ok" << std::endl;
      std::cout << "users.entry_size() " << users.entry_size() << std::endl;
      if(users.entry_size()) {
        UserData user = users.entry(0);
        std::cout << user.email() << " " << user.nickname() << " " << user.is_active() << std::endl;
      }
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      std::cout << "RPC Failed" << std::endl;
    }
  }

 private:
  std::unique_ptr<UserService::Stub> stub_;
};

void RunClient() {
  std::string target_address("0.0.0.0:9090");
  // Instantiates the client
  UserServiceClient client(
      // Channel from which RPCs are made - endpoint is the target_address
      grpc::CreateChannel(target_address,
                          // Indicate when channel is not authenticated
                          grpc::InsecureChannelCredentials()));


  client.Register();
  client.EnumerateRecords(5, 5);
  client.GetRecord('email_one');
  client.EnumerateRecords(5, 5);

  std::cout << "Done" << std::endl;
}

int main(int argc, char* argv[]) {
  RunClient();

  return 0;
}