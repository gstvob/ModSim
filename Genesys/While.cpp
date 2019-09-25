/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   While.cpp
 * Author: gstvob
 * 
 * Created on 03 de Junho de 2019, 15:20
 */

#include "While.h"
#include "Model.h"
#include "Attribute.h"
#include "Resource.h"

While::While(Model* model): ModelComponent(model, Util::TypeOf<While>()) {
   
} 

While::While(const While& orig) : ModelComponent(orig) {
}

While::~While() {
}

std::string While::show() {
    return ModelComponent::show() +
	    "";
}

PluginInformation* While::GetPluginInformation() {
    return new PluginInformation(Util::TypeOf<While>(), &While::LoadInstance);
}

ModelComponent* While::LoadInstance(Model* model, std::map<std::string, std::string>* fields) {
    While* newComponent = new While(model);
    try {
	newComponent->_loadInstance(fields);
    } catch (const std::exception& e) {
    }
    return newComponent;
}


void While::setWaitForValueExpr(std::string _expr) {
    this->_wait_for_value = _expr;
}

void While::setType(Type _type) {
    this->_type = _type;
}

void While::setQueueName(std::string _name) throw() {
    Queue* queue = dynamic_cast<Queue*>(_model->getElementManager()->getElement(Util::TypeOf<Queue>(), _name));
    if (queue != nullptr) {
        _queue = queue;
    } else {
        throw std::invalid_argument("Queue does not exist");
    }
}

While::Type While::getType() const {
    return this->_type;
}

std::string While::getWaitForValueExpr() const {
    return this->_wait_for_value;
}

std::string While::getQueueName() const {
    return _queue->getName();
}

void While::_execute(Entity* entity) {
    if (_type == Type::ScanForCondition) {
        Waiting* waiting = new Waiting(entity, this, _model->getSimulation()->getSimulatedTime());
        this->_queue->insertElement(waiting);
        double condition = _model->parseExpression((_wait_for_value));
	    _model->getTraceManager()->traceSimulation(Util::TraceLevel::blockInternal, _model->getSimulation()->getSimulatedTime(), entity, this, _wait_for_value + "the condition evaluated to " + std::to_string(condition));
        if (condition) {
            _model->sendEntityToComponent(entity, this->getNextComponents()->front(), 0.0);
            return;
        }
    }
    else if (_type == Type::WaitForSignal) {
        Queue* signal_queue = dynamic_cast<Queue*>(_model->getElementManager()->getElement(Util::TypeOf<Queue>(), _wait_for_value));
        Waiting* waiting = new Waiting(entity, this, _model->getSimulation()->getSimulatedTime());
        this->_queue->insertElement(waiting);
        signal_queue->insertElement(waiting);        
    }
}

void While::release_signal(int _limit) {
	for(int i = 0; i < _queue->size(); i++) {
		Waiting* waiting = _queue->getAtRank(i);

		_model->sendEntityToComponent(waiting->getEntity(), this->getNextComponents()->front(), 0.0);
		if (i >= _limit) {
			break;
		}
	}
}

void While::_initBetweenReplications() {
    this->_queue->initBetweenReplications();
}

bool While::_loadInstance(std::map<std::string, std::string>* fields) {
    bool res = ModelComponent::_loadInstance(fields);
    if (res) {
        
        std::string W = ((*(fields->find("queueName"))).second);
	    Queue* queue = dynamic_cast<Queue*> (_model->getElementManager()->getElement(Util::TypeOf<Queue>(), queueName));
        this->_queue = queue;
        this->_wait_for_value = ((*(fields->find("waitForValue"))).second);
    }
    return res;
}

std::map<std::string, std::string>* While::_saveInstance() {
    std::map<std::string, std::string>* fields = ModelComponent::_saveInstance(); //Util::TypeOf<Seize>());
    fields->emplace("queueName", (this->_queue->getName()));
    fields->emplace("waitForValue", (this->_wait_for_value));
    return fields;
}

bool While::_check(std::string* errorMessage) {
    return true;
}