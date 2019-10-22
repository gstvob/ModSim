/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Hold.cpp
 * Author: gstvob
 * 
 * Created on 03 de Junho de 2019, 15:20
 */

#include "Hold.h"
#include "Model.h"
#include "Attribute.h"
#include "Resource.h"
#include "Signal.h"

Hold::Hold(Model* model): ModelComponent(model, Util::TypeOf<Hold>()) {
   
} 

Hold::Hold(const Hold& orig) : ModelComponent(orig) {
}

Hold::~Hold() {
}

std::string Hold::show() {
    return ModelComponent::show() +
	    "";
}

PluginInformation* Hold::GetPluginInformation() {
    return new PluginInformation(Util::TypeOf<Hold>(), &Hold::LoadInstance);
}

ModelComponent* Hold::LoadInstance(Model* model, std::map<std::string, std::string>* fields) {
    Hold* newComponent = new Hold(model);
    try {
	newComponent->_loadInstance(fields);
    } catch (const std::exception& e) {
    }
    return newComponent;
}


void Hold::setWaitForValueExpr(std::string _expr) {
    this->_wait_for_value = _expr;
}

void Hold::setType(HoldType _type) {
    this->_type = _type;
}

void Hold::setQueue(Queue* _name) throw() {
    this->_queue = _name;
}

Hold::HoldType Hold::getType() const {
    return this->_type;
}

std::string Hold::getWaitForValueExpr() const {
    return this->_wait_for_value;
}

std::string Hold::getQueueName() const {
    return _queue->getName();
}

void Hold::_execute(Entity* entity) {
    if (_type == HoldType::ScanForCondition) {
        Waiting* waiting = new Waiting(entity, this, _model->getSimulation()->getSimulatedTime());
        this->_queue->insertElement(waiting);
        double condition = _model->parseExpression((_wait_for_value));
	    _model->getTraceManager()->traceSimulation(Util::TraceLevel::blockInternal, _model->getSimulation()->getSimulatedTime(), entity, this, _wait_for_value + "the condition evaluated to " + std::to_string(condition));
        if (condition) {
            _model->sendEntityToComponent(entity, this->getNextComponents()->front(), 0.0);
            return;
        }
    }
    else if (_type == HoldType::WaitForSignal) {
        Queue* signal_queue = dynamic_cast<Queue*>(_model->getElementManager()->getElement(Util::TypeOf<Queue>(), _wait_for_value));
        Waiting* waiting = new Waiting(entity, this, _model->getSimulation()->getSimulatedTime());
        this->_queue->insertElement(waiting);
        signal_queue->insertElement(waiting);        
    }
}

void Hold::release_signal(int _limit) {
	for(int i = 0; i < _queue->size(); i++) {
		Waiting* waiting = _queue->getAtRank(i);

		_model->sendEntityToComponent(waiting->getEntity(), this->getNextComponents()->front(), 0.0);
		if (i >= _limit) {
			break;
		}
	}
}

void Hold::_initBetweenReplications() {
    this->_queue->initBetweenReplications();
}

bool Hold::_loadInstance(std::map<std::string, std::string>* fields) {
    bool res = ModelComponent::_loadInstance(fields);
    if (res) {
        
        std::string queueName = ((*(fields->find("queueName"))).second);
	    Queue* queue = dynamic_cast<Queue*> (_model->getElementManager()->getElement(Util::TypeOf<Queue>(), queueName));
        this->_queue = queue;
        this->_wait_for_value = ((*(fields->find("waitForValue"))).second);
    }
    return res;
}

std::map<std::string, std::string>* Hold::_saveInstance() {
    std::map<std::string, std::string>* fields = ModelComponent::_saveInstance(); //Util::TypeOf<Seize>());
    fields->emplace("queueName", (this->_queue->getName()));
    fields->emplace("waitForValue", (this->_wait_for_value));
    return fields;
}

bool Hold::_check(std::string* errorMessage) {
    return true;
}