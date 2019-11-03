/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Search.h
 * Author: Grupo 5
 *
 */

#ifndef SEARCH_H
#define SEARCH_H

#include "ModelComponent.h"
#include "Plugin.h"

class Search : public ModelComponent {
public:
    Search(Model* model);
    Search(const Search& orig);
    virtual ~Search();
public:
    void setType(int _type);
    int getType() const;
    void setQueueName(std::string _queueName);
    std::string getQueueName() const;
    void setStartingRank(std::string _starting_rank);
    std::string getStartingRank() const;
    void setEndingRank(std::string _ending_rank);
    std::string getEndingRank() const;
    void setSearchCondition(std::string _search_condition);
    std::string getSearchCondition() const;
public:
    virtual std::string show();
public:
    static PluginInformation* GetPluginInformation();
    static ModelComponent* LoadInstance(Model* model, std::map<std::string, std::string>* fields);
protected:
    virtual void _execute(Entity* entity);
    virtual void _initBetweenReplications();
    virtual bool _loadInstance(std::map<std::string, std::string>* fields);
    virtual std::map<std::string, std::string>* _saveInstance();
    virtual bool _check(std::string* errorMessage);
private:
    /*
    * Type: Determination of what will be searched. Search options include entities 
    * in a queue, entities within a group (batch) or some expression(s).
    * 0 = queue; 1 = group; 2 = expression;
    */
    int _type = 0;

    /*
    * Queue Name: Name of the queue that will be searched. Applies only when the Type 
    * is Search a Queue.
    */
    std::string _queueName = "Queue";

    /*
    * Starting Value: Starting rank in the queue or group or starting value for J in an expression.
    */
    std::string _starting_rank = "1";

    /*
    * Ending Value: Ending rank in the queue or group or ending value for J in an expression.
    */
    std::string _ending_rank = "1";

    /*
    * Search Condition: Condition containing the index J for searching expressions or containing 
    * an attribute name(s) for searching queues or batches.
    */
    std::string _search_condition = "";


};

#endif /* SEARCH_H */

