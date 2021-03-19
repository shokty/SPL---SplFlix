#ifndef SESSION_H_
#define SESSION_H_
#include <vector>
#include <unordered_map>
#include <string>
#include "Action.h"

class User;
class Watchable;

class Session{
public:
    Session(const std::string &configFilePath);
    Session(const Session& sess); // Copy Constructor
    Session(Session &&other);// Move Constructor
    ~Session(); // Destructor
    Session& operator=(const Session& other); // Copy Assignment Operator
    Session& operator=(Session&& other); // Move Assignment Operator

    void start();

    void setActiveUser(User* user);
    User* getActiveUser();
    std::vector<Watchable*> getContent();
    std::unordered_map<std::string,User*>* getMap();
    std::vector<BaseAction*> getActionsLog();
    void addActionsLog(BaseAction* action);

    bool contain(std::string name);
    User* findUser(std::string name);
    std::vector<std::string>splitText(std::string action);

protected:
    void Steal(Session &other);
    void clean();
    void copy(const Session &sess);


private:
    std::vector<Watchable*> content;
    std::vector<BaseAction*> actionsLog;
    std::unordered_map<std::string,User*> userMap;
    User* activeUser;

};
#endif