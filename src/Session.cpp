//
// Created by maya on 11/18/19.
//
#include "../include/Session.h"
#include "../include/json.hpp"
#include "../include/Watchable.h"
#include "../include/User.h"
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

Session::Session(const string &configFilePath):content() , actionsLog() , userMap() , activeUser(){
    using json = nlohmann::json;
    json j;
    ifstream reader(configFilePath);
    reader >> j;
    reader.close();
    Movie* movie;
    Episode* episode;
    string name;
    int length;
    long id = 1;

    for (const auto& item : j["movies"].items()) {
        vector<string> tags;
        name = item.value()["name"];
        length = item.value()["length"];
        for (const auto& tag : item.value()["tags"].items())
            tags.push_back(tag.value());
        movie = new Movie(id ,name, length, tags);
        id++;
        content.push_back(movie);
    }
    //run over the tv_series and add them to the content
    for (const auto& item : j["tv_series"].items()){
        vector<string> tags;
        name = item.value()["name"];
        length = item.value()["episode_length"];
        int NumOfSeason = 1;
        for (const auto& tag : item.value()["tags"].items())
            tags.push_back(tag.value());
        for (const auto& season : item.value()["seasons"].items()) {
            for (int currentEpisode = 1; currentEpisode<= season.value(); currentEpisode++){
                episode = new Episode(id ,name, length , NumOfSeason , currentEpisode , tags);
                content.push_back(episode);
                id++;
                episode->setNextEpisodeId(id);
            }
            NumOfSeason++;
        }
        episode->setNextEpisodeId(0);//last episode of a tv-series
    }
    activeUser = new LengthRecommenderUser("default");//create default user
}

//destructor
Session::~Session(){
    clean();

}
void Session::clean(){
    for (size_t i = 0; i < content.size(); i++){
        delete(content[i]);
    }

    content.clear();

    if ((activeUser != nullptr) && (activeUser->getName() == "default"))
        delete(activeUser);

    for(auto it= userMap.begin(); it != userMap.end(); it++) {
        if (it->second)
            delete(it->second);
    }

    userMap.clear();

    for (size_t i = 0; i < actionsLog.size(); i++){
        delete(actionsLog[i]);
    }

    actionsLog.clear();
}

//copy constructor
Session::Session(const Session &sess) : content() , actionsLog() , userMap() , activeUser() {
    copy(sess);
}

void Session::copy(const Session &sess) {
    //copy all the content
    for (size_t i = 0; i < sess.content.size(); i++)
        content.push_back(sess.content[i]->duplicate());

    //copy the action log
    for (size_t i = 0; i < sess.actionsLog.size(); i++)
        actionsLog.push_back(sess.actionsLog[i]->duplicate());

    //change users watch history to point to the new content before they are being copied
    for(auto it=sess.userMap.begin();it!=sess.userMap.end();it++) {
        vector<Watchable*> history;
        Watchable* watchable;
        for (size_t i = 0; i < it->second->get_history().size(); i++) {
            watchable = content[(it->second->get_history()[i]->getId()) - 1];
            history.push_back(watchable);
        }
        it->second->set_history(history);
    }

    //default user is not in userMap
    if (sess.activeUser->getName() == "default"){
        vector<Watchable*> history;
        Watchable* watchable;
        for (size_t i = 0; i < sess.activeUser->get_history().size(); i++) {
            watchable = content[(sess.activeUser->get_history()[i]->getId()) - 1];
            history.push_back(watchable);
        }
        sess.activeUser->set_history(history);
    }

    //copy all the users
    activeUser = sess.activeUser->duplicate(sess.activeUser->getName());

    for(auto it=sess.userMap.begin();it!=sess.userMap.end();it++) {
        if (it->first == activeUser->getName())
            getMap()->insert({it->first , activeUser});
        else
            getMap()->insert({it->first , it->second->duplicate(it->first)});
    }

}

//copy assignment operator
Session& Session::operator=(const Session& sess){
    if (this == &sess) {
        return *this;
    }
    clean();
    copy(sess);
    return *this;
}

//move constructor
Session::Session(Session &&other):content() , actionsLog() , userMap() , activeUser(){
    Steal(other);

}

void Session::Steal(Session &other) {

    activeUser = other.activeUser;
    other.activeUser = nullptr;

    for (size_t i=0; i<other.content.size(); i++){
        content.push_back(other.content[i]);
    }

    other.content.clear();

    for (size_t i=0; i<other.actionsLog.size(); i++){
        actionsLog.push_back(other.actionsLog[i]);
    }
    other.actionsLog.clear();

    for (auto it = other.userMap.cbegin(); it != other.userMap.cend(); it++){
        userMap.insert({it->first,it->second});
        other.userMap.at(it->first) = nullptr;
    }
    other.userMap.clear();

}


//move assignment operator
Session& Session::operator=(Session&& other){
    if (this == &other) {
        return *this;
    }
    clean();
    Steal(other);
    return *this;
}


void Session::start()
{
    cout << "SPLFLIX is now on!" << endl;
    string action;
    getline(cin , action);
    vector<string> result = splitText(action);
    while(action.compare("exit") != 0){
        string command = "";
        if (action != "")
            command = result[0];
        if (command.compare("createuser") == 0){
            CreateUser* createUser = new CreateUser(result[1] , result[2]);
            createUser->act(*this);
            actionsLog.push_back(createUser);
        }
        else if (command.compare("changeuser") == 0){
            ChangeActiveUser* changeUser = new ChangeActiveUser(result[1]);
            changeUser->act(*this);
            actionsLog.push_back(changeUser);
        }
        else if (command.compare("deleteuser") == 0){

            DeleteUser* deleteUser = new DeleteUser(result[1]);
            deleteUser->act(*this);
            actionsLog.push_back(deleteUser);
        }
        else if (command.compare("dupuser") == 0){
            DuplicateUser* duplicateUser = new DuplicateUser(result[1] , result[2]);
            duplicateUser->act(*this);
            actionsLog.push_back(duplicateUser);
        }
        else if (command.compare("content") == 0){
            PrintContentList* printContentList = new PrintContentList();
            printContentList->act(*this);
            actionsLog.push_back(printContentList);
        }
        else if (command.compare("watch") == 0){
            Watch* watch = new Watch(result[1]);
            watch->act(*this);
            actionsLog.push_back(watch);
        }
        else if (command.compare("watchhist") == 0){
            PrintWatchHistory* printWatchHistory = new PrintWatchHistory();
            printWatchHistory->act(*this);
            actionsLog.push_back(printWatchHistory);
        }
        else if (command.compare("log") == 0){
            PrintActionsLog* printActionsLog = new PrintActionsLog();
            printActionsLog->act(*this);
            actionsLog.push_back(printActionsLog);
        }
        getline(cin , action);
        result = splitText(action);
    }
    Exit* exit = new Exit();
    exit->act(*this);
    actionsLog.push_back(exit);
}

//this function split to comment text based on spaces and save each word in a vector
vector<string> Session::splitText(string action) {
    vector<std::string> result;
    std::istringstream iss(action);
    for (std::string s; iss >> s;)
        result.push_back(s);
    return result;
}

void Session::setActiveUser(User* user){
    if (activeUser->getName() == "default")
        delete activeUser;
    activeUser = user;
}

User* Session::getActiveUser(){
    return activeUser;
}

vector<Watchable*> Session::getContent(){
    return content;
}

bool Session::contain(string name){
    if (userMap.find(name) == userMap.end())
        return false;
    return true;
}

User* Session::findUser(string name){
    unordered_map<string,User*>::const_iterator got = userMap.find(name);
    User* user = got->second;
    return user;
}

unordered_map<string,User*>* Session::getMap() {
    return &userMap;
}

vector<BaseAction*> Session::getActionsLog() {
    return actionsLog;
}

void Session::addActionsLog(BaseAction *action) {
    actionsLog.push_back(action);
}
