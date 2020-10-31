#include "databasemanager.h"

#include <iostream>
#include <string>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QString>
#include <QVariant>
#include <QThreadStorage>
#include <QUuid>
#include <QCryptographicHash>


void DatabaseManager::SetupConfig(const std::string p) {
    path = p.c_str();
}

bool DatabaseManager::Register(const RegisterUserData* registerUserData) {

    QSqlDatabase db = GetDatabaseThisThread();
    QSqlQuery query(db);
    query.prepare("INSERT INTO users (email, nickname, is_active, password) "
                    "VALUES (:email, :nickname, :is_active, :password)");
    query.bindValue(":email", registerUserData->data().email().c_str());
    query.bindValue(":nickname", registerUserData->data().nickname().c_str());
    query.bindValue(":is_active", registerUserData->data().is_active());

    std::string password = registerUserData->password();
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(password.c_str(), password.length());
    std::string hex = hash.result().toHex().toStdString();
    query.bindValue(":password", hex.c_str());

    bool success = query.exec();
    return success;
}

bool DatabaseManager::EnumerateRecords(const Page* page, Users* users) {

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

bool DatabaseManager::GetRecord(const PrimaryKey* key, Users* users) {

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


QSqlDatabase DatabaseManager::GetDatabaseThisThread() {    
    if(databasePool.hasLocalData()) {
        return databasePool.localData();
    } else {
        QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE", QUuid::createUuid().toString());
        database.setDatabaseName(path);
        databasePool.setLocalData(database);
        if (!database.open())
            std::cout << "Error: connection with database fail" << std::endl;
        else
            std::cout << "Database: connection ok" << std::endl;
        return database;
    }
}