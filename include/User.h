#ifndef USER_H_
#define USER_H_

#include <vector>
#include <string>
#include <unordered_set>
#include <map>

class Watchable;
class Session;

class User{
public:
    User(const std::string& name);
    virtual ~User();
    User(const User* other , const std::string &name);
    virtual Watchable* getRecommendation(Session& s) = 0;
    virtual User* duplicate(std::string name);
    std::string getName() const;
    std::vector<Watchable*> get_history() const;
    void add_history(Watchable* watch);
    void set_history(std::vector<Watchable*> otherHistory);

protected:
    std::vector<Watchable*> history;
    bool hasWatched(Watchable* watch);
private:
    const std::string name;

};


class LengthRecommenderUser : public User {
public:
    LengthRecommenderUser(const std::string& name);
    LengthRecommenderUser(LengthRecommenderUser *user, const std::string &name);
    virtual LengthRecommenderUser* duplicate(std::string name);
    virtual Watchable* getRecommendation(Session& s);
private:

};

class RerunRecommenderUser : public User {
public:
    RerunRecommenderUser(const std::string& name);
    RerunRecommenderUser(RerunRecommenderUser *user, const std::string &name);
    virtual RerunRecommenderUser* duplicate(std::string name);
    virtual Watchable* getRecommendation(Session& s);
private:
    int lastRecommendedId;
};

class GenreRecommenderUser : public User {
public:
    GenreRecommenderUser(const std::string& name);
    GenreRecommenderUser(GenreRecommenderUser *user, const std::string &name);
    virtual GenreRecommenderUser* duplicate(std::string name);
    virtual Watchable* getRecommendation(Session& s);
    std::string getBestTag(std::map<std::string , int> tagsCount);
private:
};

#endif