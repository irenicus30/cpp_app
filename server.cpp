#include <grpcpp/grpcpp.h>
#include <sstream>
#include <string>
#include "myapi.grpc.pb.h"


#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QString>
#include <QVariant>
#include <QThreadStorage>
#include <QUuid>

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
  static DatabaseManager& GetInstance() {
    static DatabaseManager instance;
    return instance;
  }

  void SetupConfig(const std::string p) {
    path = p.c_str();
  }

  bool Register(const RegisterUserData* registerUserData) {

    QSqlDatabase db = GetDatabaseThisThread();
    QSqlQuery query(db);
    query.prepare("INSERT INTO users (email, nickname, is_active, password) "
                  "VALUES (:email, :nickname, :is_active, :password)");
    query.bindValue(":email", registerUserData->data().email().c_str());
    query.bindValue(":nickname", registerUserData->data().nickname().c_str());
    query.bindValue(":is_active", registerUserData->data().is_active());
    query.bindValue(":password", registerUserData->password().c_str());
    bool success = query.exec();
    return success;
  }

  bool EnumerateRecords(const Page* page, Users* users) {

    int page_number = page->page_number();
    int page_size = page->page_size();
    int offset = (page_number-1) * page_size;
    if(offset<0) {
      std::cout << "Negative offset! page numeration should start from 1" << std::endl;
      return false;
    }

    QSqlDatabase db = GetDatabaseThisThread();
    QSqlQuery query(db);
    query.prepare("SELECT * FROM users LIMIT :offset,:page_size");
    query.bindValue(":offset", offset);
    query.bindValue(":page_size", page_size);
    bool success = query.exec();

    int id_email = query.record().indexOf("email");
    int id_nickname = query.record().indexOf("nickname");
    int id_is_active = query.record().indexOf("is_active");

    while (query.next()) {
      std::string email = query.value(id_email)
                               .toString()
                               .toUtf8()
                               .constData();
      std::string nickname = query.value(id_nickname)
                                  .toString()
                                  .toUtf8()
                                  .constData();
      bool is_active = query.value(id_is_active)
                            .toBool();
      UserData* data = users->add_entry();
      data->set_email(email.c_str());
      data->set_nickname(nickname.c_str());
      data->set_is_active(is_active);
    }
    return success;
  }

  bool GetRecord(const PrimaryKey* key, Users* users) {

    QSqlDatabase db = GetDatabaseThisThread();
    QSqlQuery query(db);
    query.prepare("SELECT * FROM users WHERE email = :email");
    query.bindValue(":email", key->email().c_str());
    bool success = query.exec();

    int id_email = query.record().indexOf("email");
    int id_nickname = query.record().indexOf("nickname");
    int id_is_active = query.record().indexOf("is_active");

    if (query.next()) {
      std::string email = query.value(id_email)
                               .toString()
                               .toUtf8()
                               .constData();
      std::string nickname = query.value(id_nickname)
                                  .toString()
                                  .toUtf8()
                                  .constData();
      bool is_active = query.value(id_is_active)
                            .toBool();
      UserData* data = users->add_entry();
      data->set_email(email.c_str());
      data->set_nickname(nickname.c_str());
      data->set_is_active(is_active);
    }
    return success;
  }

  DatabaseManager(const DatabaseManager &) = delete;
  DatabaseManager & operator = (const DatabaseManager &) = delete;

private:
  QSqlDatabase GetDatabaseThisThread() {    
    if(databasePool.hasLocalData()) {
      std::cout << "database already exists" << std::endl;
      return databasePool.localData();
    } else {
      QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE", QUuid::createUuid().toString());
      database.setDatabaseName(path);
      databasePool.setLocalData(database);
      if (!database .open()) {
        std::cout << "Error: connection with database fail" << std::endl;
      } else {
        std::cout << "Database: connection ok" << std::endl;
      }
      return database;
    }
  }

  DatabaseManager() {}
  ~DatabaseManager() {}

  QThreadStorage<QSqlDatabase> databasePool;
  QString path;
};



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

int main(int argc, char** argv) {
  DatabaseManager& instance =  DatabaseManager::GetInstance();
  instance.SetupConfig("../../test.db");
  RunServer();
  return 0;
}