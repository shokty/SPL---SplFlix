//
// Created by maya on 11/18/19.
//
#include "../include/User.h"
#include "../include/Watchable.h"
#include "../include/Session.h"
#include <map>
#include <limits>

using namespace std;

//User Rule of 5
//constructor
User::User(const std::string& name): history() , name(name){

}

//copy constructor
User::User(const User* other , const std::string &name):history() , name(name){
    for (size_t i=0; i<other->get_history().size(); i++)
        history.push_back(other->get_history()[i]);
}

User::~User() {
    history.clear();
}

Watchable* User::getRecommendation(Session& s){
    return nullptr;
}

string User::getName()const{
    return name;
}

vector<Watchable*> User::get_history()const{
    return history;
}

void User::add_history(Watchable* watch){
    history.push_back(watch);
}

//constructor
LengthRecommenderUser::LengthRecommenderUser(const string& name) : User(name) {

}


LengthRecommenderUser::LengthRecommenderUser(LengthRecommenderUser* other , const string& name) : User(other , name) {

}

LengthRecommenderUser* LengthRecommenderUser::duplicate(std::string name) {
    return new LengthRecommenderUser(this , name);;
}

//find the next recommended content to watched based on Length Algorithm
Watchable* LengthRecommenderUser::getRecommendation(Session& s){
    int sum = 0;
    for (size_t i = 0; i < history.size(); i++)//calculate the average length of previous content user had watched
        sum += history[i]->getLength();
    int averageLength = sum / history.size();
    int bestIndex = -1;
    int bestLength;
    for (size_t i = 0; ((i < s.getContent().size()) & (bestIndex == -1)); i++)//find the content with the closest length to this average
        if (!hasWatched(s.getContent()[i]))/*if user hasn't watched this content yes*/ {
            bestLength = abs(s.getContent()[i]->getLength() - averageLength);
            bestIndex = i;
        }
    if (bestIndex != -1){
        for (size_t j = bestIndex+1; j < s.getContent().size(); j++)
            if (!hasWatched(s.getContent()[j]))
                if (abs(s.getContent()[j]->getLength() - averageLength) < bestLength) {
                    bestLength = abs(s.getContent()[j]->getLength() - averageLength);
                    bestIndex = j;
                }
        return (s.getContent()[bestIndex]);
    }

    return nullptr;
}

RerunRecommenderUser::RerunRecommenderUser(const std::string& name): User(name) , lastRecommendedId(0){

}


RerunRecommenderUser::RerunRecommenderUser(RerunRecommenderUser* other , const string& name) : User(other , name) , lastRecommendedId(other->lastRecommendedId){

}

RerunRecommenderUser* RerunRecommenderUser::duplicate(std::string name) {
    RerunRecommenderUser* newUser= new RerunRecommenderUser(this , name);
    return  newUser;
}

//find the next recommended content to watched based on Rerun Algorithm
Watchable* RerunRecommenderUser::getRecommendation(Session& s){
    Watchable* next = history[lastRecommendedId];
    lastRecommendedId = (lastRecommendedId + 1)%history.size();
    return next;
}

//constructor
GenreRecommenderUser::GenreRecommenderUser(const std::string &name): User(name){

}

GenreRecommenderUser::GenreRecommenderUser(GenreRecommenderUser* other , const string& name) : User(other , name) {

}

GenreRecommenderUser* GenreRecommenderUser::duplicate(std::string name) {
    GenreRecommenderUser* newUser= new GenreRecommenderUser(this , name);
    return  newUser;
}

Watchable* GenreRecommenderUser::getRecommendation(Session& s){
    if (history.size() != 0) {
        map<string, int> tagsCount;
        string tag;
        for (size_t i = 0; i < history.size(); i++) {//sum up the appearance of each tag
            for (size_t j = 0; j < history[i]->getTags().size(); j++) {
                tag = history[i]->getTags()[j];
                if (tagsCount.find(tag) == tagsCount.end())//tag does not exist already
                    tagsCount.insert({tag, 1});
                else {
                    tagsCount.find(tag)->second++;
                }
            }
        }

        string bestTag = getBestTag(tagsCount);
        tagsCount.erase(bestTag);

        while (bestTag != "") {
            for (size_t i = 0; i < s.getContent().size(); i++) {
                if (!hasWatched(s.getContent()[i]))/*if user hasn't watched this content yes*/ {
                    for (size_t j = 0; j < s.getContent()[i]->getTags().size(); j++)
                        if (s.getContent()[i]->getTags()[j] == bestTag)
                            return s.getContent()[i];
                }
            }
            bestTag = getBestTag(tagsCount);
            tagsCount.erase(bestTag);
        }
    }
    return nullptr;
}

string GenreRecommenderUser::getBestTag(map<string , int> tagsCount){
    string mostUsedTag = "";
    int usesOfBestTag = 0;
    for(auto it=tagsCount.begin();it!=tagsCount.end();it++) {
        if(it->second > usesOfBestTag){
            usesOfBestTag = it->second;
            mostUsedTag = it->first;
        }
    }
    return mostUsedTag;
}

//this function return true if the user already watched a show or false if he hasn't.
bool User::hasWatched(Watchable* watch){
    for (size_t i = 0; i < history.size(); i++)
        if (history[i] == watch)
            return true;
    return false;
}

User *User::duplicate(std::string name) {
    return nullptr;
}

void User::set_history(std::vector<Watchable *> otherHistory) {
    history =otherHistory;
}


