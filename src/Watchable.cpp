//
// Created by maya on 11/18/19.
//
#include "../include/Watchable.h"
#include "../include/Session.h"
#include "../include/User.h"

using namespace std;

Watchable::Watchable(long id, int length, const std::vector<std::string>& tags):id(id), length(length), tags(tags) {

}

Watchable::Watchable(Watchable* other):id(other->id), length(other->length) , tags(){
   for (size_t i = 0; i < other->tags.size(); i++)
       tags.push_back(other->tags[i]);
}

int Watchable::getLength(){
    return length;
}

const long Watchable::getId(){
    return id;
}

vector<string> Watchable::getTags(){
    return tags;
}

Movie::Movie(long id, const std::string& name, int length, const std::vector<std::string>& tags) : Watchable(id, length , tags)  , name(name) {

}

Movie::Movie(Movie* other):Watchable(other) , name(other->name)
{

}

Watchable* Movie::getNextWatchable(Session& s) const {
    Watchable* next = s.getActiveUser()->getRecommendation(s);
    return next;
}

Movie *Movie::duplicate() {
    return new Movie(this);
}

string Movie::toString() const {
    return name;
}

Episode::Episode(long id, const std::string& seriesName,int length, int season, int episode ,const std::vector<std::string>& tags) : Watchable(id, length , tags) , seriesName(seriesName)  , season(season) , episode(episode) , nextEpisodeId(0){

}

Episode::Episode(Episode * other):Watchable(other) , seriesName(other->seriesName)  , season(other->season) , episode(other->episode) , nextEpisodeId(other->nextEpisodeId){

}

void Episode::setNextEpisodeId(long id){
    nextEpisodeId = id;
}

Watchable* Episode::getNextWatchable(Session& s) const {
    Watchable* next;
    if (nextEpisodeId != 0){
        next = s.getContent()[nextEpisodeId-1];
    }
    else {
        next = s.getActiveUser()->getRecommendation(s);
    }
    return next;
}


string Episode::toString() const {
    string toPrint = seriesName + " S"  + to_string(season) + "E" + to_string(episode);
    return toPrint;
}

Episode *Episode::duplicate() {
    return new Episode(this);
}


Watchable::~Watchable() {
  tags.clear();
}

Watchable *Watchable::duplicate() {
    return nullptr;
}

