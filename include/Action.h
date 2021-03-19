#ifndef ACTION_H_
#define ACTION_H_

#include <string>
#include <iostream>
#include <vector>

class Session;

enum ActionStatus{
    PENDING, COMPLETED, ERROR
};


class BaseAction{
public:
    BaseAction();
    explicit BaseAction(BaseAction *action);
    virtual ~BaseAction();
    ActionStatus getStatus() const;
    std::string getErrorMsg() const;

    virtual void act(Session& sess)=0;
    void complete();
    void error(const std::string& errorMsg);
    virtual std::string toString() const=0;
    virtual BaseAction* duplicate();
private:
    std::string errorMsg;
    ActionStatus status;
};

class CreateUser  : public BaseAction {
public:
    CreateUser(std::string name , std::string algorithm);
    CreateUser(CreateUser*);
    virtual void act(Session& sess);
    virtual std::string toString() const;
    virtual CreateUser* duplicate();

private:
    std::string name;
    std::string algorithm;
};

class ChangeActiveUser : public BaseAction {
public:
    ChangeActiveUser(std::string name);
    ChangeActiveUser(ChangeActiveUser*);
    virtual void act(Session& sess);
    virtual std::string toString() const;
    virtual ChangeActiveUser* duplicate();

private:
    std::string name;
};

class DeleteUser : public BaseAction {
public:
    DeleteUser(std::string name);
    DeleteUser(DeleteUser*);
    virtual void act(Session & sess);
    virtual std::string toString() const;
    virtual DeleteUser* duplicate();

private:
    std::string name;
};


class DuplicateUser : public BaseAction {
public:
    DuplicateUser(std::string otherName , std::string myName);
    DuplicateUser(DuplicateUser* action);
    virtual void act(Session & sess);
    virtual std::string toString() const;
    virtual DuplicateUser* duplicate();

private:
    std::string otherName;
    std::string myName;
};

class PrintContentList : public BaseAction {
public:
    PrintContentList();
    PrintContentList(PrintContentList* action);
    virtual void act (Session& sess);
    virtual std::string toString() const;
    virtual PrintContentList* duplicate();
};

class PrintWatchHistory : public BaseAction {
public:
    PrintWatchHistory();
    PrintWatchHistory(PrintWatchHistory* action);
    virtual void act (Session& sess);
    virtual std::string toString() const;
    virtual PrintWatchHistory* duplicate();
};


class Watch : public BaseAction {
public:
    Watch(std::string id);
    Watch(Watch* action);
    virtual void act(Session& sess);
    virtual std::string toString() const;
    virtual Watch* duplicate();

private:
    std::string id;
};


class PrintActionsLog : public BaseAction {
public:
    PrintActionsLog();
    PrintActionsLog(PrintActionsLog* action);
    virtual void act(Session& sess);
    virtual std::string toString() const;
    virtual PrintActionsLog* duplicate();
};

class Exit : public BaseAction {
public:
    Exit();
    Exit(Exit* action);
    virtual void act(Session& sess);
    virtual std::string toString() const;
    virtual Exit* duplicate();
};
#endif
