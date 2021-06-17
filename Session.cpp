#include <istream>
#include <unordered_map>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <string>
#include <iostream>

#include "../include/Session.h"
#include "../include/json.hpp"
#include "../include/User.h"
#include "../include/Action.h"
#include "../include/Watchable.h"

using namespace std;
using namespace nlohmann;

    // Constructor
    Session :: Session(const std::string &configFilePath): content(), actionsLog(), userMap(), activeUser() {
        ifstream content_file(configFilePath);
        json j;
        content_file >> j;
        long id = 1;
    // insert movies that contain length and tags to the content vector
        for (auto &m : j["movies"].items()){
            json aMovie = m.value();
            int length = aMovie["length"];
            vector<string> tags;
            for (auto &t : aMovie["tags"]){
                string tag = t;
                tags.push_back((tag));
            }
            // push the item in the vector
            auto* to_add = new Movie (id, (string)aMovie["name"],length,tags);
            this->content.push_back(to_add);
            id = id+1;
        }
    // insert the episodes to the content vector
        for (auto &s : j["tv_series"].items()) {
            json aShow = s.value();
            int length = aShow["episode_length"];
            vector<string> tags;
            for (auto &t: aShow["tags"]) {
                string tag = t;
                tags.push_back((tag));
            }
            int s_count = 1;
            for (auto &season : aShow["seasons"]) {
                for (int i = 1; i <= season; i = i + 1) {
                    auto *to_add = new Episode(id, (string) aShow["name"], length, s_count, i, tags);
                    this->content.push_back(to_add);
                    id = id + 1;
                }
                s_count = s_count + 1;
            }
        }
        // creat a current activeUser and add it to the userMap
        User* default_user = new LengthRecommenderUser("default");
        userMap.insert({"default", default_user});
        activeUser = default_user;
    }
    //Copy Constructor
    Session :: Session (const Session &other):content(), actionsLog(), userMap(), activeUser() {
        for(Watchable* con : other.content) {
            content.push_back(con->Clone());
        }
        for(BaseAction* base : other.actionsLog) {
            actionsLog.push_back(base->Clone());
        }
        for(pair<string,User*> us : other.userMap){
            User* u = us.second->Clone();
            u->get_history().clear();
            for (Watchable* x : us.second->get_history()) {
                int index = x->getId()-1;
                u->get_history().push_back(content.at(index));
            }
            userMap.insert({us.first, u});
        }
        activeUser = userMap[other.activeUser->getName()];
    }
    // Copy Assignment
    Session &Session::operator=(const Session &other) {
        if (this != &other){
            this->ClearSession();
            for(Watchable* con : other.content) {
                content.push_back(con->Clone());
            }
            for(BaseAction* base : other.actionsLog) {
                actionsLog.push_back(base->Clone());
            }
            for(pair<string,User*> us : other.userMap){
                User* u = us.second->Clone();
                u->get_history().clear();
                for (Watchable* x : us.second->get_history()) {
                    int index = x->getId()-1;
                    u->get_history().push_back(content.at(index));
                }
                userMap.insert({us.first, u});
            }
            activeUser = userMap[other.activeUser->getName()];
        }
        return *this;
    }
    // Destructor
    Session :: ~Session(){
        ClearSession();
    }

    // Move Constructor
    Session :: Session(Session &&other):content(), actionsLog(), userMap(), activeUser() {
        for(Watchable* con : other.content) {
            content.push_back(con);
            con = nullptr;
        }
        for(BaseAction* base : other.actionsLog) {
            actionsLog.push_back(base);
            base = nullptr;
        }
        activeUser = userMap[other.activeUser->getName()];
        for(pair<string,User*> us : other.userMap){
            userMap.insert({us.first, us.second});
            us.second = nullptr;
        }
        delete(&other);
    }
    // Move Assignment
    Session &Session::operator=(Session &&other) {
        if (this != &other){
            this->ClearSession();
            for(Watchable* con : other.content) {
                content.push_back(con);
                con = nullptr;
            }
            for(BaseAction* base : other.actionsLog) {
                actionsLog.push_back(base);
                base = nullptr;
            }
            activeUser = userMap[other.activeUser->getName()];
            for(pair<string,User*> us : other.userMap){
                userMap.insert({us.first, us.second});
                us.second = nullptr;
            }
            delete(&other);
        }
        return *this;
    }

    void Session::ClearSession() {
            //delete all items in content
            for(auto& content_item : content) {
                delete (content_item);
            }
            content.clear();
            //delete all items in actionLog and then delete actionLog
            for (auto& act: actionsLog)
                delete(act);
            actionsLog.clear();
            //delete active user
            activeUser = nullptr;
            //delete userMap
            /// i changed it here !!!
            for (auto& us: userMap){
                delete (us.second);
            }
            userMap.clear();
        }

    void Session :: start(){
        cout << "SPLFLIX is now on!" << endl;

        bool terminate = true;
        // the active session loop
        while(terminate){
            // get the user input from the terminal
            string userInput;
            getline(cin, userInput);
            string action = userInput.substr(0,userInput.find(' '));

            if (action == "createuser"){
                userInput = userInput.substr(11);
                string user_name;
                user_name = userInput.substr(0,userInput.find(' '));
                string algorithm;
                algorithm = userInput.substr(user_name.size()+1);

                // check if the user_name already exist in the map
                if(findUser(user_name)) user_name = "name error";

                // check if the algorithm input is fine
                if ((algorithm != "len") && (algorithm != "rer") && (algorithm != "gen")) algorithm = "alg error";

                // send the details to the function to act
                auto* x = new CreateUser(user_name, algorithm);
                (*x).act(*this);

                // add the action to the action log
                actionsLog.push_back(x);

                // print the action status and commends
                cout << (*x).toString() << endl;

            }
            else if (action == "changeuser"){
                userInput = userInput.substr(11);
                string user_name;
                user_name = userInput.substr(0,userInput.find(' '));

                // send the details to the function to act
                auto* x = new ChangeActiveUser(user_name);
                (*x).act(*this);

                // add the action to the action log
                actionsLog.push_back(x);

                // print the action status and commends
                cout << (*x).toString() << endl;

            }
            else if (action == "deleteuser"){
                userInput = userInput.substr(11);
                string user_name;
                user_name = userInput.substr(0,userInput.find(' '));

                // send the details to the function to act
                auto* x = new DeleteUser(user_name);
                (*x).act(*this);

                // add the action to the action log
                actionsLog.push_back(x);

                // print the action status and commends
                cout << (*x).toString() << endl;
            }
            else if (action == "dupuser"){
                userInput = userInput.substr(8);
                string original_user_name;
                original_user_name = userInput.substr(0,userInput.find(' '));
                string new_user_name;
                new_user_name = userInput.substr(original_user_name.size()+1);

                // send the details to the function to act
                auto* x = new DuplicateUser(original_user_name, new_user_name);
                (*x).act(*this);

                // add the action to the action log
                actionsLog.push_back(x);

                // print the action status and commends
                cout << (*x).toString() << endl;
            }
            else if (action == "content"){
                // send the details to the function to act
                auto* x = new PrintContentList();
                (*x).act(*this);

                // add the action to the action log
                actionsLog.push_back(x);

                // print the action status and commends
                cout << (*x).toString() << endl;
            }
            else if (action == "watchhist"){
                // send the details to the function to act
                auto* x = new  PrintWatchHistory();
                (*x).act(*this);

                // add the action to the action log
                actionsLog.push_back(x);

                // print the action status and commends
                cout << (*x).toString() << endl;
            }
            else if (action == "watch"){
                // get the id of the wanted content
                userInput = userInput.substr(6);

                // send the details to the function to act
                auto* x = new  Watch(userInput);
                (*x).act(*this);

                // print the action status and commends
                cout << (*x).toString() << endl;
            }
            else if (action == "log"){
                // send the details to the function to act
                auto* x = new  PrintActionsLog();
                (*x).act(*this);

                // add the action to the action log
                actionsLog.push_back(x);

                // print the action status and commends
                cout << (*x).toString() << endl;
            }
            else if (action == "exit") {
                terminate = false;

                // send the details to the function to act
                auto* x = new  Exit();
                (*x).act(*this);

                // add the action to the action log
                actionsLog.push_back(x);

                // print the action status and commends
                cout << (*x).toString() << endl;

                break;
            }
            else {cout << "invalid input" << endl;}
        }

    }


    void Session :: change_active_user (User* to_change){ activeUser = to_change;}

    void Session :: delete_user (string to_delete) {
        if (!(userMap.find(to_delete) == userMap.end())){
            delete(userMap.find(to_delete)->second);
            userMap.erase(to_delete);
        }
    }

    User* Session :: getActiveUser() const {return activeUser;}

    vector<Watchable*>& Session :: getContent() {return content;}

    vector<BaseAction*>& Session :: getLog(){return actionsLog;}
    unordered_map<string, User *> &Session::getUserMap() {return userMap;}

    bool Session :: findUser (string& name){
        for (auto&x : userMap) if (x.first == name) return true;
    return false;
    }

    User* Session :: findUserInMap (string& name){
        for (auto& x: userMap) {
            if (x.first == name) return x.second;
        }
        return nullptr;
    }
    void Session :: createUser (string name, string algorithm){
        User* new_user;
        if (algorithm == "len")
            new_user = new LengthRecommenderUser(name);
        else if (algorithm == "rer")
            new_user = new RerunRecommenderUser(name);
        else{ // (algorithm == "gen")
            new_user =  new GenreRecommenderUser(name);}
        userMap.insert({name, new_user});
    }








