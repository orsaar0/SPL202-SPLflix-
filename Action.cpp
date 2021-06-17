//#include "../include/Action.h"
#include "../include/User.h"
#include "../include/Session.h"
#include "../include/Action.h"
#include <string>


using namespace std;

    // BaseAction Class
    BaseAction ::BaseAction(): errorMsg(""), status(PENDING){}
    BaseAction :: ~BaseAction () = default;
    ActionStatus BaseAction:: getStatus() const { return status;}
    void BaseAction:: act(Session& sess){}
    string BaseAction:: toString() const {
        if (status == PENDING) return "PENDING";
        else if (status == COMPLETED) return "COMPLETED";
        else return "ERROR: " + errorMsg;
    }
    void BaseAction:: complete() {status = COMPLETED;}
    void BaseAction:: error(const string& message){
         BaseAction::errorMsg = message;
         status = ERROR;
    }
    string BaseAction::getErrorMsg() const {return errorMsg;}

    // CreatUser Class
    CreateUser:: CreateUser (string name, string algorithm):  BaseAction(), name(name), algorithm(algorithm) {}
    CreateUser ::~CreateUser() = default;
    void CreateUser::  act (Session& sess){
        if (name == "name error") error("the new user name is already taken");
        else if (algorithm == "alg error") error("desired algorithm type is not exist");
        else {

            sess.createUser(name, algorithm);
            complete();
        }
    }
    string CreateUser:: toString() const {
        if (getStatus() == ERROR) return "CreateUser ERROR: " + getErrorMsg();
        else return "CreateUser COMPLETED";
    }
    CreateUser* CreateUser :: Clone() const{
        return (new CreateUser (*this));
    }

    // ChangeActiveUser
    ChangeActiveUser::ChangeActiveUser(string& name): BaseAction(), name(name) {}
    ChangeActiveUser::~ChangeActiveUser() = default;

    void ChangeActiveUser:: act (Session& sess){
        // wanted user is not found
        if (!sess.findUser(name)) error("user to active is not exist");

        // user is found, put it as an activeUser in sess
        else {
            sess.change_active_user(sess.findUserInMap(name));
            complete();
        }
    }
    string ChangeActiveUser:: toString() const {
        if (getStatus() == ERROR) return "ChangeActiveUser ERROR: " + getErrorMsg();
        else return "ChangeActiveUser COMPLETED";
    }
    ChangeActiveUser* ChangeActiveUser :: Clone() const{
        return (new ChangeActiveUser (*this));
    }

    // DeleteUser Class
    DeleteUser:: DeleteUser (string to_delete): BaseAction(), to_delete(to_delete){}
    DeleteUser ::~DeleteUser() = default;
    void DeleteUser :: act (Session& sess){
        // wanted user is not found
        if (!sess.findUser(to_delete)) error("user for deletion is not found");

        // user is found, delete it from user Map
        else {
            sess.delete_user(to_delete);
            complete();
        }
    }
    string DeleteUser ::toString() const {
        if (getStatus() == ERROR) return "DeleteUser ERROR: " + getErrorMsg();
        else return "DeleteUser COMPLETED";
    }
    DeleteUser* DeleteUser :: Clone() const{
        return (new DeleteUser (*this));
    }

    //DuplicateUser Class
    DuplicateUser :: DuplicateUser (string original_user, string new_user):  BaseAction(), o(original_user), n(new_user){}
    DuplicateUser ::~DuplicateUser() = default;
    void DuplicateUser :: act (Session& sess) {
        if (!sess.findUser(o)) error("original user is missing");
        else {
            // creat a new user on the heap with the same algorithm
            sess.createUser(n, sess.findUserInMap(o)->getData());
            // set the history of the new user to be tha same as original
            sess.findUserInMap(n)->setHistory(sess.findUserInMap(o)->get_history());
            complete();
        }
    }
    string DuplicateUser :: toString() const {
        if (getStatus() == ERROR) return "DuplicateUser ERROR: " + getErrorMsg();
        else return "DuplicateUser COMPLETED";
    }
    DuplicateUser* DuplicateUser :: Clone() const{
        return (new DuplicateUser (*this));
    }

    // PrintContentList Class
    PrintContentList :: PrintContentList (): BaseAction(){}
    PrintContentList ::~PrintContentList() = default;
    void PrintContentList :: act (Session& sess){
        for(auto& x : sess.getContent()) {
            string to_print = (*x).toString();
            cout << to_print << endl;
        }
        complete();
    }
    string PrintContentList ::toString() const {
        return "PrintContentList COMPLETED";
    }
    PrintContentList* PrintContentList :: Clone() const{
        return (new PrintContentList (*this));
    }

    // PrintWatchHistory Class
    PrintWatchHistory :: PrintWatchHistory ():  BaseAction() {}
    PrintWatchHistory ::~PrintWatchHistory() = default;
    void PrintWatchHistory :: act (Session& sess){
        if (sess.getActiveUser() -> get_history().size() == 0) error("history is empty");
        else {
            int index = 1;
            for (auto& x: sess.getActiveUser()->get_history()) {
                cout << to_string(index)+ ". " + x->contentToString()  << endl;
                index++;
            }
            complete();
        }
    }
    string PrintWatchHistory ::toString() const {
        if (getStatus() == ERROR) return "PrintWatchHistory ERROR: " + getErrorMsg();
        else return "PrintWatchHistory COMPLETED";
    }
    PrintWatchHistory* PrintWatchHistory :: Clone() const{
            return (new PrintWatchHistory (*this));
    }

    // Watch Class
    Watch :: Watch (string user_input): BaseAction(), id(move(user_input)){}
    Watch ::~Watch() = default;
    void Watch :: act (Session& sess) {
        // add the action to the action log
        Watchable* recommend = nullptr;
        // search the content that the user wants to watch
        int input_id = stoi(id);
        for (auto& x : sess.getContent()){
            if(input_id == x->getId()){
                cout << "Watching " + x->toString() << endl;
                sess.getActiveUser()->get_history().push_back(x);
                complete();
                sess.getLog().push_back(this);
                recommend = x->getNextWatchable(sess);
                break;
            }
        }

        if (recommend == nullptr) cout << "no recommendation for you now" << endl;
        else {
            cout << "We recommend watching " + recommend->toString() + ", continue watching? [y/n]" << endl;
            string ans;
            getline(cin, ans);
            if (ans == "y") {
                auto *a = new Watch(to_string(recommend->getId()));
                (*a).act(sess);
            } else {
                if (ans != "n")
                    cout << "ok... we guess you didn't like our recommendation, don't give up on us! let's start over again" << endl;
            }
        }
    }
    string Watch ::toString() const {
        if (getStatus() == ERROR) return "PrintWatchHistory ERROR: " + getErrorMsg();
        else return "Watch COMPLETED";
    }
    Watch* Watch :: Clone() const{
        return (new Watch (*this));
    }

    // PrintActionsLog Class
    PrintActionsLog :: PrintActionsLog (): BaseAction(){}
    PrintActionsLog ::~PrintActionsLog() = default;
    void PrintActionsLog :: act (Session& sess){
        string final_string;
        if (sess.getLog().size() == 0) error("log is empty");
        else {
            for (BaseAction* x: sess.getLog()) final_string = x->toString() + "\n" + final_string;
            cout << final_string << endl;
            complete();
        }
    }
    string PrintActionsLog :: toString() const {
        if (getStatus() == ERROR) return "PrintActionsLog ERROR: " + getErrorMsg();
        else return "PrintActionsLog COMPLETED";
    }
    PrintActionsLog* PrintActionsLog :: Clone() const{
        return (new PrintActionsLog (*this));
    }

    // Exit Class
    Exit :: Exit (): BaseAction(){}
    Exit ::~Exit() = default;
    void Exit :: act (Session& sess){
        cout << "Tnx for using SPLFLIX! hope to see you again soon. Goodbye" << endl;
        complete();
    }   

    string Exit :: toString() const {
        return "EXIT COMPLETED";
    }
    Exit* Exit :: Clone() const{
        return (new Exit (*this));
    }
//todo: delete Session? what should we do here?...

