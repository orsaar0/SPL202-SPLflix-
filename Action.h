#ifndef ACTION_H_
#define ACTION_H_

#include <string>
#include <iostream>
#include "../include/Watchable.h"

class Session;
class User;
using namespace std;

enum ActionStatus{
	PENDING, COMPLETED, ERROR
};

class BaseAction{
public:
	BaseAction();
	virtual ~BaseAction() ;
	ActionStatus getStatus() const;
	virtual void act(Session& sess)=0;
	virtual std::string toString() const=0;
    virtual BaseAction* Clone() const=0;
protected:
	void complete();
	void error(const std::string& message);
	string getErrorMsg() const;
private:
	string errorMsg;
	ActionStatus status;
};

class CreateUser  : public BaseAction {
public:
    CreateUser(string name, string algorithm);
    ~CreateUser();
    virtual void act(Session& sess);
	virtual string toString() const;
	CreateUser* Clone() const;
private:
    string name;
    string algorithm;
};

class ChangeActiveUser : public BaseAction {
public:
    ChangeActiveUser(string& name);
    ~ChangeActiveUser();
	virtual void act(Session& sess);
	virtual std::string toString() const;
    ChangeActiveUser* Clone() const;
private:
    string name;
};

class DeleteUser : public BaseAction {
public:
    DeleteUser (string name);
    ~DeleteUser();
	virtual void act(Session & sess);
	virtual std::string toString() const;
    DeleteUser* Clone() const;
private:
    string to_delete;
};


class DuplicateUser : public BaseAction {
public:
    DuplicateUser (string o, string n);
    ~DuplicateUser();
	virtual void act(Session & sess);
	virtual std::string toString() const;
    DuplicateUser* Clone() const;
private:
    string o;
    string n;
};

class PrintContentList : public BaseAction {
public:
    PrintContentList ();
    ~PrintContentList();
	virtual void act (Session& sess);
	virtual std::string toString() const;
    PrintContentList* Clone() const;
};

class PrintWatchHistory : public BaseAction {
public:
    PrintWatchHistory ();
    ~PrintWatchHistory();
    virtual void act (Session& sess);
	virtual std::string toString() const;
    PrintWatchHistory* Clone() const;
};


class Watch : public BaseAction {
public:
    Watch (string id);
    ~Watch();
	virtual void act(Session& sess);
	virtual std::string toString() const;
    Watch* Clone() const;
private:
    string id;
};


class PrintActionsLog : public BaseAction {
public:
    PrintActionsLog ();
    ~PrintActionsLog();
	virtual void act(Session& sess);
	virtual std::string toString() const;
    PrintActionsLog* Clone() const;
private:
};

class Exit : public BaseAction {
public:
    Exit ();
    ~Exit();
	virtual void act(Session& sess);
	virtual std::string toString() const;
    Exit* Clone() const;
};
#endif
