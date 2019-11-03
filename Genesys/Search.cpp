/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Search.cpp
 * Author: Grupo 5
 * 
 */

/*
* The Search module searches a queue, a group (batch), or an expression 
* to find the entity rank (for entities in a queue or group) or the value 
* of the global variable J that satisfies the specified search condition.
*
*   When searching a queue or group, the value of the global system variable 
*   J is set to the rank of the first entity that satisfies Search Condition, 
*   or to 0 if Search Condition is not satisfied.
*
*   When searching an expression, the global system variable J is set to the 
*   value of the first index value that satisfies the search condition or to 
*   zero if no value of J in the specified range satisfies the search condition.
*/

#include "Search.h"
#include "Model.h"
#include "Queue.h"
#include "Group.h"
#include "Variable.h"
#include <stdio.h>
#include <string>

Search::Search(Model* model) : ModelComponent(model, Util::TypeOf<Search>()) {
}

Search::Search(const Search& orig) : ModelComponent(orig) {
}

Search::~Search() {
}

std::string Search::show() {
    return ModelComponent::show() +
    ",type=" + std::to_string(this->_type) +
    ",starting rank=" + this->_starting_rank +
    ",ending rank=" + this->_ending_rank +
    ",search condition=" + this->_search_condition;
}

ModelComponent* Search::LoadInstance(Model* model, std::map<std::string, std::string>* fields) {
    Search* newComponent = new Search(model);
    try {
	   newComponent->_loadInstance(fields);
    } catch (const std::exception& e) {

    }
    return newComponent;
}

std::string Search::getStartingRank() const {
    return this->_starting_rank;
}

std::string Search::getEndingRank() const {
    return this->_ending_rank;
}

void Search::setStartingRank(std::string rank) {
    this->_starting_rank = rank;
}

void Search::setEndingRank(std::string rank) {
    this->_ending_rank = rank;
}

void Search::setQueueName(std::string _queueName) {
    this->_queueName = _queueName;
}

std::string Search::getQueueName() const {
    return _queueName;
}

void Search::setType(int type) {
    this->_type = type;
}

int Search::getType() const {
    return this->_type;
}

void Search::setSearchCondition(std::string condition) {
    this->_search_condition = condition;
}

std::string Search::getSearchCondition() const {
    return _search_condition;
}

/*
* When an entity arrives at a Search module, the index J is set to the starting 
* index and the search condition is then checked.
* 
* If the search condition is satisfied, the search ends and the current value of 
* J is retained. Otherwise, the value of J is increased or decreased and the condition is rechecked.
*
* This process repeats until the search condition is satisfied or the ending value is reached. If 
* the condition is not met or there are no entities in the queue or group, J is set equal to 0.
*/
void Search::_execute(Entity* entity) {

    Variable* j = dynamic_cast<Variable*> (_model->getElementManager()->getElement(Util::TypeOf<Variable>(), "Variable_J"));

    _model->getTraceManager()->trace(Util::TraceLevel::blockInternal, "X4 search component show: " + this->show());

    int beg_rank = _model->parseExpression(this->getStartingRank());
    int end_rank = _model->parseExpression(this->getEndingRank());

    double condition = _model->parseExpression(this->getSearchCondition());

    bool found = false;

    if(this->getType() == 0) { //queue
        /* Gets the specified queue from its name */ 
        Queue* queue = dynamic_cast<Queue*> (_model->getElementManager()->getElement(Util::TypeOf<Queue>(), this->getQueueName()));
        int queue_size = queue->size();

        if(beg_rank < end_rank) {
            int i;
            for(; beg_rank <= end_rank; beg_rank++) {
                /* Get the element (entity) within the queue */
                Waiting* element = queue->getAtRank(beg_rank);

                if(element == nullptr) {
                    continue;
                }

                /* Find the time in queue of the element */
                double tnow = _model->getSimulation()->getSimulatedTime();
                double timeInQueue = element->getTimeStartedWaiting();

                /* If condition is true, break the loop and show that found */
                if(timeInQueue > condition) {
                    _model->getTraceManager()->trace(Util::TraceLevel::blockInternal, "======== FOUND ========");
                    found = true;
                    break;
                }
            } 
        } else if (beg_rank > end_rank) {
            int i;
            for(; beg_rank >= end_rank; beg_rank--) {
                /* Get the element (entity) within the queue */
                Waiting* element = queue->getAtRank(beg_rank);

                if(element == nullptr) {
                    continue;
                }

                /* Find the time in queue of the element */
                double tnow = _model->getSimulation()->getSimulatedTime();
                double timeInQueue = element->getTimeStartedWaiting();

                /* If condition is true, break the loop and show that found */
                if(timeInQueue > condition) {
                    _model->getTraceManager()->trace(Util::TraceLevel::blockInternal, "======== FOUND ========");
                    found = true;
                    break;
                }
            }
        } else {
            /* Get the element (entity) within the queue */
            Waiting* element = queue->getAtRank(beg_rank);

            if(element != nullptr) {
                /* Find the time in queue of the element */
                double tnow = _model->getSimulation()->getSimulatedTime();
                double timeInQueue = element->getTimeStartedWaiting();

                /* If condition is true, break the loop and show that found */
                if(timeInQueue > condition) {
                    _model->getTraceManager()->trace(Util::TraceLevel::blockInternal, "======== FOUND ========");
                    found = true;
                }
            }
        }
    } else if (this->getType() == 1) { //batch

        /* Get the group of entities */
        std::string copy_number = std::to_string(entity->getAttributeValue("Entity.CopyNumber"));
        std::string entity_name = entity->getEntityTypeName();
        std::string group_count = std::to_string((unsigned int) entity->getAttributeValue("Entity.GroupRankID"));
        std::string attribute_value = std::to_string(entity->getAttributeValue("Entity.AttributeValue"));
        std::string group_id = entity_name + group_count + attribute_value + copy_number;
        Group* group = (Group*) _model->getElementManager()->getElement(Util::TypeOf<Group>(), group_id);

        if (group == nullptr) {
            _model->getTraceManager()->trace(Util::TraceLevel::blockInternal, "Group doesn't exist!");
            return;
        }

        if(beg_rank < end_rank) {
            int i;
            for(; beg_rank <= end_rank; beg_rank++) {
                /* Get the element (entity) within the group */
                Entity* element = group->_list->getAtRank(beg_rank);

                if(element == nullptr) {
                    continue;
                }

                /* If condition is true, break the loop and show that found */
                if(condition) {
                    _model->getTraceManager()->trace(Util::TraceLevel::blockInternal, "======== FOUND ========");
                    found = true;
                    break;
                }
            } 
        } else if (beg_rank > end_rank) {
            int i;
            for(; beg_rank >= end_rank; beg_rank--) {
                /* Get the element (entity) within the group */
                Entity* element = group->_list->getAtRank(beg_rank);

                if(element == nullptr) {
                    continue;
                }

                /* If condition is true, break the loop and show that found */
                if(condition) {
                    _model->getTraceManager()->trace(Util::TraceLevel::blockInternal, "======== FOUND ========");
                    found = true;
                    break;
                }
            }
        } else {
            Entity* element = group->_list->getAtRank(beg_rank);

            if(element != nullptr) {

                /* If condition is true, break the loop and show that found */
                if(condition) {
                    _model->getTraceManager()->trace(Util::TraceLevel::blockInternal, "======== FOUND ========");
                    found = true;
                }
            }
        }
    } else { //expression

        /* Get the 1D Variable */
        Variable* variable = dynamic_cast<Variable*> (_model->getElementManager()->getElement(Util::TypeOf<Variable>(), this->getQueueName()));

        if(beg_rank < end_rank) { // forward search
            int i;
            for(; beg_rank < end_rank; beg_rank++) {

                double k = variable->getValue(std::to_string(beg_rank));

                /* Evaluating search condition */
                double condition = _model->parseExpression(this->getSearchCondition());

                /* If condition is true, break the loop and show that found */
                if(k > condition) {
                    _model->getTraceManager()->trace(Util::TraceLevel::blockInternal, "########## FOUND ##########");
                    found = true;
                    break;
                }
            }
        } else if(beg_rank > end_rank) {
            int i;
            for(; beg_rank >= end_rank; beg_rank--) {

                double k = variable->getValue(std::to_string(beg_rank));

                /* Evaluating search condition */
                double condition = _model->parseExpression(this->getSearchCondition());

                /* If condition is true, break the loop and show that found */
                if(k > condition) {
                    _model->getTraceManager()->trace(Util::TraceLevel::blockInternal, "########## FOUND ##########");
                    found = true;
                    break;
                }
            }
        } else {
            double k = variable->getValue(std::to_string(beg_rank));

            /* Evaluating search condition */
            double condition = _model->parseExpression(this->getSearchCondition());

            /* If condition is true, break the loop and show that found */
            if(k > condition) {
                _model->getTraceManager()->trace(Util::TraceLevel::blockInternal, "########## FOUND ##########");
                found = true;
            }
        }
    }

    /* If found, set the global system variable to the rank */
    /* Depends if found or not, send the current entity to the next component */
    if(found) {
        j->setValue(beg_rank);
        this->_model->sendEntityToComponent(entity, this->getNextComponents()->getAtRank(0), 0.0);
    } else {
        j->setValue(0.0);
        this->_model->sendEntityToComponent(entity, this->getNextComponents()->getAtRank(1), 0.0);
    }
}

bool Search::_loadInstance(std::map<std::string, std::string>* fields) {
    bool res = ModelComponent::_loadInstance(fields);
    if (res) {
        this->_type = std::stoi(((*(fields->find("type"))).second));
        this->_starting_rank = ((*(fields->find("starting_rank"))).second);
        this->_ending_rank = ((*(fields->find("ending_rank"))).second);
        this->_search_condition = ((*(fields->find("search_condition"))).second);
    }
    return res;
}

void Search::_initBetweenReplications() {
    Variable* j = dynamic_cast<Variable*> (_model->getElementManager()->getElement(Util::TypeOf<Variable>(), "Variable_J"));
    j->setValue(0.0);
}

std::map<std::string, std::string>* Search::_saveInstance() {
    std::map<std::string, std::string>* fields = ModelComponent::_saveInstance(); 
    fields->emplace("type", std::to_string(this->_type));
    fields->emplace("starting_rank", this->_starting_rank);
    fields->emplace("ending_rank", this->_ending_rank);
    fields->emplace("search_condition", this->_search_condition);
    return fields;
}

bool Search::_check(std::string* errorMessage) {
    bool resultAll =  true;
    resultAll &= _model->checkExpression(_starting_rank, "starting_rank", errorMessage);
    resultAll &= _model->checkExpression(_ending_rank, "ending_rank", errorMessage);
    resultAll &= _model->checkExpression(_search_condition, "search_condition", errorMessage);
    return resultAll;
}

PluginInformation* Search::GetPluginInformation() {
    return new PluginInformation(Util::TypeOf<Search>(), &Search::LoadInstance);
}
