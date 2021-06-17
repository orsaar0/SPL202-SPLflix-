#include "../include/User.h"

    //Abstract Class User
    User::User(const std::string& name):history(), name(name){}
    User :: ~User() = default;
    string User:: getName() const {
        return name;
    }

    vector<Watchable *> &User::get_history() {
        return history;
}

    void User :: setHistory(vector<Watchable *> &historyOther) {
        if (!historyOther.empty()){
            int index = historyOther.size();
            for (int i = 0; i< index; ++i)
                history.push_back(historyOther[i]);
        }
    }

//LengthRecommenderUser
    LengthRecommenderUser :: LengthRecommenderUser(const std::string& name):User(name){};
    string LengthRecommenderUser :: getData(){
        return "len";
    }

    Watchable* LengthRecommenderUser :: getRecommendation(Session& s){
        int sum = 0;
        int average = 0;
        std::vector<Watchable*>& hisList = s.getActiveUser()->get_history();
        std::vector<Watchable*>& conList = s.getContent();
        Watchable* chosen = nullptr;
        if (!hisList.empty()){   //Calculates the user's average length
            int index = hisList.size();
            for (int i = 0; i < index; ++i){
                sum = sum + hisList[i]->get_Length();
            }
            average = (sum/index);
        }
        int index = conList.size();
        for (int i = 0; i < index; ++i){   //Searches the content with a length closest to average
            bool notfound = true;
            if (!hisList.empty()){
                int index1 = hisList.size();
                for (int j = 0; (notfound) & (j < index1); ++j) {
                    if (conList[i]->getId() == hisList[j]->getId()){
                        notfound = false;
                    }
                }
            }
            if (notfound){
                if (average == conList[i]->get_Length()){
                    return conList[i];
                };
                if (chosen == nullptr)
                    chosen = conList[i];
                if (abs(average - conList[i]->get_Length()) < abs(average - chosen->get_Length())){
                    chosen = conList[i];
                }
            }
        }
        return chosen;
    }

    User *LengthRecommenderUser::Clone() const {
        return new LengthRecommenderUser(*this);
    }
//todo: assure chosen not null in the function's caller


    //RerunRecommenderUser
    RerunRecommenderUser :: RerunRecommenderUser(const std::string& name):User(name), counter(){
        counter=0;
    }
    string RerunRecommenderUser :: getData(){return "rer";}

    User *RerunRecommenderUser::Clone() const {
        return new RerunRecommenderUser(*this);
    }
    Watchable* RerunRecommenderUser ::getRecommendation(Session& s){
        std::vector<Watchable*> hisList = s.getActiveUser()->get_history();
        if (hisList.empty()){
            return nullptr;
        }
        Watchable* toReturn = hisList[(counter) % hisList.size()];
        counter++;
        return toReturn;
    }



//GenreRecommenderUser
    GenreRecommenderUser :: GenreRecommenderUser(const std::string& name):User(name),popular(){}
    string GenreRecommenderUser :: getData(){return "gen";}
    User *GenreRecommenderUser::Clone() const {
        return new GenreRecommenderUser(*this);
    }
    Watchable* GenreRecommenderUser :: getRecommendation(Session& s){
        if (popular.empty())
            CreatePop(s);
        return PopularGenre(s);
    }
    void GenreRecommenderUser::CreatePop(Session &s) {
        std::vector<Watchable*> hisList = s.getActiveUser()->get_history();
        if (!hisList.empty()){
            int index = hisList.size();
            for (int i = 0 ; i < index; i++){
                std::vector<std::string>& tags_ins = hisList[i]->getTags();
                int index = tags_ins.size();
                for (int j = 0 ; j < index; j++){
                    UpdatePop(tags_ins[j]);
                }
            }
        }
    }
    Watchable *GenreRecommenderUser::PopularGenre(Session &s) {
        std::vector<Watchable*>& hisList = s.getActiveUser()->get_history();
        std::vector<Watchable*>& conList = s.getContent();
        //loop on the genre from most popular tag > down
        int index = popular.size();
        for (int i = 0; i<index; i++){
            string fav = popular[i].second;
            //find a content with the most popular tag
            if (!conList.empty()){
                for (Watchable* con : conList){
                    for(string tag : con->getTags()){
                        if (fav == tag){
                            //check if the content hasn't been watched yet
                            bool notfound = true;
                            if (!hisList.empty()){
                                int index = hisList.size();
                                for (int j = 0; (notfound) & (j < index); ++j) {
                                    if (con->getId() == hisList[j]->getId()){
                                        notfound = false;
                                    }
                                }
                                //if the user hasn't watched con content before
                                if (notfound){
                                    return con;
                                }
                            }
                        }
                    }
                }
            }
        }
        return 0;
    }

    void GenreRecommenderUser::InsertPop(std::string tag) {
        popular.emplace_back(1,tag);
        OrderPop(popular.size()-1);
    }

    void GenreRecommenderUser::SwapTags(int i) {
        if (i > 0){
            string temp = popular[i-1].second;
            popular[i-1].second = popular[i].second;
            popular[i].second = temp;
        }
    }

    void GenreRecommenderUser::IncreaseTag(int i) {
        popular[i].first++;
        OrderPop(i);
    }

    void GenreRecommenderUser::OrderPop(int i) {
        if (i > 0){
            if (popular[i-1].first < popular[i].first) {
                SwapTags(i - 1);
                OrderPop(i-1);
            }
            else if (popular[i-1].first == popular[i].first){
                if (popular[i-1].second < popular[i].second){
                    SwapTags(i-1);
                    OrderPop(i-1);
                }
            }
        }
    }

    void GenreRecommenderUser::UpdatePop(const string& tag) {
        bool found = false;
        int index = popular.size();
        for (int i = 0; (!found) && (i < index); i++){
            if (popular[i].second == tag){
                IncreaseTag(i);
                found = true;
            }
        }
        if (!found)
            InsertPop(tag);
    }



