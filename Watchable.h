#ifndef WATCHABLE_H_
#define WATCHABLE_H_

#include <string>
#include <vector>
#include "../include/Session.h"
#include "../include/User.h"
#include <sstream>
#include <iostream>
#include <iterator>
#include <string.h>
using namespace std;


class Session;

class Watchable{
public:
    Watchable(long id, int length, const std::vector<std::string>& tags);
    virtual ~Watchable();
    virtual std::string toString() const = 0;
    virtual Watchable* getNextWatchable(Session& s) const = 0;
    virtual string contentToString() = 0;
    int get_Length() const;
    int getId() const;
    std::vector<std::string>& getTags();
    virtual string get_name() const =0;
    Watchable(const Watchable& other);
    virtual Watchable* Clone() const = 0;
private:
    const long id;
    int length;
    std::vector<std::string> tags;
};

class Movie : public Watchable{
public:
    Movie(long id, const std::string& name, int length, const std::vector<std::string>& tags);
    ~Movie();
    virtual std::string toString() const;
    virtual string contentToString();
    virtual Watchable* getNextWatchable(Session& s) const;
    string get_name() const;
    Movie(const Movie &other);
    virtual Watchable* Clone() const;
private:
    std::string name;
};

class Episode: public Watchable{
public:
    Episode(long id, const std::string& seriesName, int length, int season, int episode, const std::vector<std::string> &tags);
    ~Episode();
    virtual std::string toString() const;
    virtual string contentToString();
    virtual Watchable* getNextWatchable(Session& s) const;
    string get_name() const;
    Episode(const Episode& other);
    virtual Watchable* Clone() const;
private:
    std::string seriesName;
    int season;
    int episode;
};

#endif
