//
// Created by maya on 11/18/19.
//
#include <sstream>
#include <vector>
#include "../include/Action.h"
#include "../include/User.h"
#include "../include/Session.h"
#include "../include/Watchable.h"
#include <string>

using namespace std;

BaseAction::BaseAction(): errorMsg(""), status(PENDING) {

}

BaseAction::BaseAction(BaseAction* action): errorMsg("") , status(action->status) {

}

void BaseAction::act(Session &sess) {

}


ActionStatus BaseAction::getStatus() const {
    return status;
}

void BaseAction::complete() {
    status = COMPLETED;
}

void BaseAction::error(const std::string &errorMsg) {
    this->errorMsg = errorMsg;
    status = ERROR;
}

string BaseAction::getErrorMsg() const {
    return errorMsg;
}

CreateUser::CreateUser(string name , string algorithm):name(name) , algorithm(algorithm){

}

void CreateUser::act(Session &sess) {
    if ((algorithm.length() == 3) &&
    //create user based on the wanted algorithm
    ((algorithm == "len") | (algorithm == "rer") | (algorithm == "gen"))) {
        if (!sess.contain(name)) {
            if (algorithm == "len") {
                LengthRecommenderUser* newUser = new LengthRecommenderUser(name);
                sess.getMap()->insert({name, newUser});
            } else if (algorithm == "rer") {
                RerunRecommenderUser* newUser = new RerunRecommenderUser(name);
                sess.getMap()->insert({name, newUser});
            } else {
                GenreRecommenderUser* newUser = new GenreRecommenderUser(name);
                sess.getMap()->insert({name, newUser});
            }
            complete();
        } else{
            error("user already exist");
        }
    }
    else
        error("wrong algorithm name");
}

ChangeActiveUser::ChangeActiveUser(string name):name(name){

}

void ChangeActiveUser::act(Session &sess) {
    if (sess.contain(name)) {
        sess.setActiveUser(sess.findUser(name));
        complete();
    } else {
        error("this user doesn't exist");
    }
}

DeleteUser::DeleteUser(string name):name(name){

}

void DeleteUser::act(Session &sess) {
    if (sess.contain(name)) {
        delete sess.findUser(name);
        sess.getMap()->erase(name);
        complete();
    } else {
        error("didn't find a user with this name to delete");
    }
}

DuplicateUser::DuplicateUser(string otherName , string myName):otherName(otherName) , myName(myName){

}

void DuplicateUser::act(Session &sess) {
    if (sess.contain(otherName) ) {
        if (!sess.contain(myName)) {
            sess.getMap()->insert({myName, sess.findUser(otherName)->duplicate(myName)});
            complete();
        } else
            error("user already exist");
    } else {
        error("this user to duplicate from doesn't exist");
    }
}

void PrintContentList::act(Session &sess) {
    for (size_t i = 0; i < sess.getContent().size(); i++) {
        Watchable *currentContent = sess.getContent()[i];
        cout << currentContent->getId() << ". " << currentContent->toString() << " " << currentContent->getLength()
             << " minutes [";
        for (size_t j = 0; j < currentContent->getTags().size(); j++) {
            cout << currentContent->getTags()[j];
            if (j < currentContent->getTags().size() - 1)
                cout << ", ";
        }
        cout << "]" << "\n";
    }
    complete();
}

void PrintWatchHistory::act(Session &sess) {
    cout << "watch history for " << sess.getActiveUser()->getName() << "\n";
    for (size_t i = 0; i < sess.getActiveUser()->get_history().size(); i++) {
        cout << (i + 1) << ". " << sess.getActiveUser()->get_history()[i]->toString() << "\n";
    }
    complete();
}

Watch::Watch(string id):id(id){

}

void Watch::act(Session &sess) {
    Watchable *watch = sess.getContent()[(std::stoi(id) - 1)];
    sess.getActiveUser()->add_history(watch);
    cout << "watching now " << watch->toString() << "\n";
    Watchable *next = watch->getNextWatchable(sess);
    if (next != nullptr) {
        cout << "We recommend watching " << next->toString() << ", continue watching? [y/n]" << "\n";
        string answer;
        getline(cin, answer);
        if (answer == "y") {
            complete();
            id = to_string(next->getId());
            Watch* watch = new Watch(id);
            watch->act(sess);
            sess.addActionsLog(watch);
        } else complete();
    } else complete();
}


void PrintActionsLog::act(Session &sess) {
    vector<BaseAction *> actionLog = sess.getActionsLog();
    for (size_t i = actionLog.size(); i > 0; i--) {
        cout << actionLog[i-1]->toString() + " ";
        ActionStatus status = actionLog[i-1]->getStatus();
        if (status==COMPLETED)
            cout << "COMPLETED";
        else if (status==PENDING)
            cout << "PENDING";
        else if (status == ERROR)
            cout << actionLog[i-1]->getErrorMsg();
        cout <<  "\n";
    }
    complete();
}  //Printing all user action logs from end to start



void Exit::act(Session &sess) {
    complete();
}

// ToString for every action start here:

string BaseAction::toString() const {
    return "";
}

BaseAction *BaseAction::duplicate() {
    return nullptr;
}

BaseAction::~BaseAction() {

}

string CreateUser::toString() const {
    return "CreateUser";
}

CreateUser *CreateUser::duplicate(){
    return new CreateUser(this);;
}

CreateUser::CreateUser(CreateUser* action):BaseAction(action) , name(action->name) , algorithm(action->algorithm) {

}

string DeleteUser::toString() const {

    return "DeleteUser";
}

DeleteUser *DeleteUser::duplicate() {
    return new DeleteUser(this);
}

DeleteUser::DeleteUser(DeleteUser* action):BaseAction(action) , name(action->name) {

}

string DuplicateUser::toString() const {

    return "DuplicateUser";
}

DuplicateUser *DuplicateUser::duplicate() {
    return new DuplicateUser(this);
}

DuplicateUser::DuplicateUser(DuplicateUser *action):BaseAction(action) , otherName(action->otherName) , myName(action->myName){

}

string PrintWatchHistory::toString() const {

    return "PrintWatchHistory";
}

PrintWatchHistory *PrintWatchHistory::duplicate() {
    return new PrintWatchHistory(this);
}

PrintWatchHistory::PrintWatchHistory(PrintWatchHistory *action):BaseAction(action) {

}

PrintWatchHistory::PrintWatchHistory() {

}

string ChangeActiveUser::toString() const {

    return "ChangeActiveUser";
}

ChangeActiveUser *ChangeActiveUser::duplicate() {
    return new ChangeActiveUser(this);
}

ChangeActiveUser::ChangeActiveUser(ChangeActiveUser* action):BaseAction(action) , name(action->name){

}

string PrintContentList::toString() const {

    return "PrintContentList";
}

PrintContentList *PrintContentList::duplicate() {
    return new PrintContentList(this);
}

PrintContentList::PrintContentList(PrintContentList *action) :BaseAction(action){

}

PrintContentList::PrintContentList() {

}

string Watch::toString() const {
    return "Watch";
}

Watch *Watch::duplicate() {
    return new Watch(this);
}

Watch::Watch(Watch *action) :BaseAction(action) , id(action->id){

}

string PrintActionsLog::toString() const {

    return "PrintActionsLog";
}

PrintActionsLog *PrintActionsLog::duplicate() {
    return new PrintActionsLog(this);
}

PrintActionsLog::PrintActionsLog(PrintActionsLog *action) :BaseAction(action){

}

PrintActionsLog::PrintActionsLog(){

}

string Exit::toString() const {

    return "Exit";
}

Exit *Exit::duplicate() {
    return new Exit(this);
}

Exit::Exit(){

}

Exit::Exit(Exit *action):BaseAction(action) {

}

