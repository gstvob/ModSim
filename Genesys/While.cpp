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

/*
    Verifies the expression, if true, continue execution, else, send entity
    to the component which endwhile is connected
*/
void While::_execute(Entity* entity) {
    auto value = _model->parseExpression(_condition);
    if (value) {
        _model->sendEntityToComponent(entity, this->getNextComponents()->front(), 0.0);
    } else {
        _model->sendEntityToComponent(entity, _attached_endwhile->getNextComponents()->front(), 0.0);
    }
}


void While::_initBetweenReplications() {}

bool While::_loadInstance(std::map<std::string, std::string>* fields) {
    bool res = ModelComponent::_loadInstance(fields);
    return res;
}

std::map<std::string, std::string>* While::_saveInstance() {
    std::map<std::string, std::string>* fields = ModelComponent::_saveInstance(); //Util::TypeOf<Seize>());
    return fields;
}

bool While::_check(std::string* errorMessage) {
    return true;
}