#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include "myapi.grpc.pb.h"

#include <QSqlDatabase>
#include <QString>
#include <QThreadStorage>

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
    
    DatabaseManager(const DatabaseManager &) = delete;
    DatabaseManager & operator = (const DatabaseManager &) = delete;

    void SetupConfig(const std::string p);

    bool Register(const RegisterUserData* registerUserData);

    bool EnumerateRecords(const Page* page, Users* users);

    bool GetRecord(const PrimaryKey* key, Users* users);

private:
    DatabaseManager() {}
    ~DatabaseManager() {}

    QSqlDatabase GetDatabaseThisThread();

    QThreadStorage<QSqlDatabase> databasePool;
    QString path;
};

#endif