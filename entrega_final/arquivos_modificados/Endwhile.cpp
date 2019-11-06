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

#include "Endwhile.h"
#include "Model.h"
#include "Attribute.h"
#include "Resource.h"

Endwhile::Endwhile(Model* model): ModelComponent(model, Util::TypeOf<Endwhile>()) {
   
} 

Endwhile::Endwhile(const Endwhile& orig) : ModelComponent(orig) {
}

Endwhile::~Endwhile() {
}

std::string Endwhile::show() {
    return ModelComponent::show() + " ";
}

PluginInformation* Endwhile::GetPluginInformation() {
    return new PluginInformation(Util::TypeOf<Endwhile>(), &Endwhile::LoadInstance);
}

ModelComponent* Endwhile::LoadInstance(Model* model, std::map<std::string, std::string>* fields) {
    Endwhile* newComponent = new Endwhile(model);
    try {
	newComponent->_loadInstance(fields);
    } catch (const std::exception& e) {
    }
    return newComponent;
}

void Endwhile::_execute(Entity* entity) {
    _model->sendEntityToComponent(entity, _attached_while, 0.0);
}


void Endwhile::_initBetweenReplications() {}

bool Endwhile::_loadInstance(std::map<std::string, std::string>* fields) {
    bool res = ModelComponent::_loadInstance(fields);
    return res;
}

std::map<std::string, std::string>* Endwhile::_saveInstance() {
    std::map<std::string, std::string>* fields = ModelComponent::_saveInstance();
    return fields;
}

bool Endwhile::_check(std::string* errorMessage) {
    return true;
}