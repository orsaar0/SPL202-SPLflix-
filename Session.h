#ifndef SESSION_H_
#define SESSION_H_

#include <vector>
#include <unordered_map>
#include <string>
#include <fstream>
#include "../include/Action.h"


//#include "include/json.hpp"

class User;
class Watchable;
using namespace std;

class Session{
public:
    // Constructor
    Session(const std::string &configFilePath);
    // Copy Constructor
    Session(const Session& other);
    // Copy Assignment
    Session& operator=( const Session &other);
    // Destructor
    virtual ~Session();
    // Move Constructor
    Session(Session&& other);
    // Move Assignment
    Session& operator=(Session &&other);
    void ClearSession();
    void start();
    User* getActiveUser() const;
    vector<BaseAction*>& getLog();
    vector<Watchable*>& getContent();
    bool findUser(string& name);
    User* findUserInMap (string& name);
    unordered_map<string,User*>& getUserMap();
  //  virtual void insertUser (const string name, User* to_add);
    virtual void change_active_user (User* to_change);
    virtual void delete_user(string to_delete);
    virtual void createUser (string name, string algorithm);
private:
    vector<Watchable*> content;
    vector<BaseAction*> actionsLog;
    unordered_map<string,User*> userMap;
    User* activeUser;
};
#endif
