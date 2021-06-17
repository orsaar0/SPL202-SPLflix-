#ifndef USER_H_
#define USER_H_
#include "../include/Watchable.h"
#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>
using namespace std;
class Watchable;
class Session;

class User{
public:
    User(const std::string& name);
    virtual ~User();
   /* User(const User& other);*/
    virtual Watchable* getRecommendation(Session& s) = 0;
    std::string getName() const;
    std::vector<Watchable *> &get_history();
    virtual string getData() = 0;
    void setHistory (vector<Watchable*>& historyOther);
    virtual User* Clone() const = 0;

protected:
    std::vector<Watchable*> history;
private:
    const std::string name;

};

class LengthRecommenderUser : public User {
public:
    LengthRecommenderUser(const std::string& name);
    virtual Watchable* getRecommendation(Session& s);
    string getData();
    virtual User* Clone() const;
private:
};

class RerunRecommenderUser : public User {
public:
    RerunRecommenderUser(const std::string& name);
    virtual Watchable* getRecommendation(Session& s);
    string getData();
    virtual User* Clone() const;

private:
    int counter;
};

class GenreRecommenderUser : public User {
public:
    GenreRecommenderUser(const std::string& name);
    virtual Watchable* getRecommendation(Session& s);
    string getData();
    virtual User* Clone() const;
private:
    std::vector<std::pair<int,std::string>> popular;
    void CreatePop(Session& s);
    Watchable* PopularGenre (Session& s);
    void InsertPop(std::string tag);
    void SwapTags(int i);
    void IncreaseTag(int i);
    void UpdatePop(const std::string& tag);
    void OrderPop(int i);
};

#endif
