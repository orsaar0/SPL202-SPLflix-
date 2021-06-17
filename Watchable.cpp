#include "../include/Watchable.h"


//Class Watchable
    Watchable :: Watchable(long id, int length, const vector<string>& tags):id(id), length(length), tags(tags){}
    Watchable :: ~Watchable() = default;
    Watchable::Watchable(const Watchable &other):id(other.id), length(other.length), tags() {
        for(string tag : other.tags) {
            tags.push_back(tag);
        }
    }
    int Watchable :: get_Length() const{
        return length;
    }
    int Watchable :: getId() const{
        return id;
    }
    std::vector<std::string>& Watchable::getTags() {
        std::vector<std::string>& tagsToReturn = tags;
        return tagsToReturn;
    }


//Class Movie
    Movie :: Movie (long id, const string &name, int length, const vector<string> &tags):Watchable(id, length, tags), name(name){}
    //Copy Constructor
    Movie::Movie(const Movie &other) : Watchable(other), name(other.name){}
    Movie :: ~Movie()= default;
    std::string Movie:: toString() const {
       return (std::string(name));
    }
    string Movie::contentToString() {
        string content = toString() + " " + to_string(get_Length()) + " minutes [";
        vector<string>& tagsToP = getTags();
        int index =  tagsToP.size();
        for (int i=0; i< index; i++){
            content = content + tagsToP[i] + ", ";
       }
        content = content.substr(0,content.size()-2) + "]";

        return content;
    }

    Watchable* Movie :: getNextWatchable(Session& s) const {
        return (s.getActiveUser()->getRecommendation(s));
    }
    Watchable *Movie::Clone() const{
        return new Movie(*this);
    }

    string Movie ::get_name() const {return name;}

//Class Episode
    Episode :: Episode (long id, const string& seriesName, int length, int season, int episode, const std::vector<std::string> &tags): Watchable(id, length, tags), seriesName(seriesName), season(season), episode(episode) {}
    Episode::Episode(const Episode &other) :  Watchable(other), seriesName(other.seriesName), season(other.season), episode(other.episode) {}
    Episode ::~Episode() = default;
    std::string Episode:: toString() const {
        return string(seriesName + " S" + to_string(season) + "E" + to_string(episode));
    }
    string Episode :: get_name() const {return (seriesName);}

    Watchable* Episode :: getNextWatchable(Session& s) const {
        int index = getId();
        int content_size = s.getContent().size();
        if (index != content_size) {
            if (seriesName == s.getContent()[index]->get_name())
                return (s.getContent()[getId()]);
            else{ return (s.getActiveUser()->getRecommendation(s));}
        }
        else{ return (s.getActiveUser()->getRecommendation(s));}
    }
    Watchable *Episode::Clone() const{
        return new Episode(*this);
    }

    string Episode::contentToString(){
        string content = toString() + " " + to_string(get_Length()) + " minutes [";
        vector<string>& tagsToP = getTags();
        int index =  tagsToP.size();
        for (int i=0; i < index; i++){
            content = content + tagsToP[i] + ", ";
        }
        content = content.substr(0,content.size()-2) + "]";

        return content;
    }



