/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Separate.cpp
 * Author: rlcancian
 * 
 * Created on 03 de Junho de 2019, 15:14
 */

#include "Separate.h"

#include "Model.h"
#include "EntityGroup.h"

#include <string>
#include <list>
#include <assert.h>
#include "Attribute.h"

Separate::Separate(Model* model) : ModelComponent(model, Util::TypeOf<Separate>()) {
}

Separate::Separate(const Separate& orig) : ModelComponent(orig) {
}

Separate::~Separate() {
}

std::string Separate::show() {
    return ModelComponent::show() + "";
}

ModelComponent* Separate::LoadInstance(Model* model, std::map<std::string, std::string>* fields) {
    Separate* newComponent = new Separate(model);
    try {
	newComponent->_loadInstance(fields);
    } catch (const std::exception& e) {

    }
    return newComponent;
}

/*!
 *  Set the name of the separate to be created when spliting entities.
 */
void Separate::setSeparateName(std::string value) {
    this->_separateName = value;
}

/*!
 *    Set the type of the separate, whether it must split existing batch or
 *  duplicate the incoming entity
 */
void Separate::setSplitBatch(bool value) {
    this->_splitBatch = value;
}

/*!
 *  Set the number of duplicates to an entity to be generated, when using
 * duplication
 */
void Separate::setAmountToDuplicate(std::string value) {
    this->_amountToDup = value;
}

/*!
 *  Set how to assign the representative entity attribute values to the
 *  original entities
 */
void Separate::setTakeAllRepresentativeValues(bool value) {
	this->_takeAllRepresentativeValues = value;
}

/*!
 *  Execute the default behavior of the Separate component, which receives an
 * entity with a Batch of entities. It must split the original batch or
 * duplicate it, based on it's parameters. Can receive either a Batch entity
 * or any other entity
 */
void Separate::_execute(Entity* entity) {
    double currentTime = _model->getSimulation()->getSimulatedTime();
    unsigned int batchesCount = (unsigned int) entity->getAttributeValue("Entity.GroupRankID");
    double copyId = entity->getAttributeValue("Entity.CopyNumber");
    std::string batchEntityName = entity->getEntityTypeName();
    std::string batchesCountStr = std::to_string(batchesCount);
    std::string attributeValue = std::to_string(entity->getAttributeValue("Entity.AttributeValue"));

    std::string batchEntityID = batchEntityName + batchesCountStr + attributeValue + std::to_string(copyId);

    ElementManager* elementManager = this->_model->getElementManager();
    TraceManager* traceManager = this->_model->getTraceManager();

    traceManager->trace(Util::TraceLevel::blockInternal, "Arrival of entity " + std::to_string(entity->getId()) +
                                                         " at time " + std::to_string(currentTime));

    EntityGroup* group = (EntityGroup*) elementManager->getElement(Util::TypeOf<EntityGroup>(), batchEntityID);
    
    double replicationsNumber = 0;
    if(this->_attributeType){
        replicationsNumber = entity->getAttributeValue(this->_amountToDup);
    }else{
        replicationsNumber = _model->parseExpression(this->_amountToDup);
    }
    if (this->_splitBatch) {
      /* Split the existing Batch entity */
      traceManager->trace(Util::TraceLevel::blockInternal, "Split the existing Batch entity");
      Entity* batchEntity;

      if ((bool)entity->getAttributeValue("Entity.Permanent")) {
          this->_model->sendEntityToComponent(entity, this->getNextComponents()->front(), 0.0);
          return;
      }

      assert(group != nullptr);

      for (auto i = 0u; i < group->size(); i++) {
          batchEntity = group->first();

          group->removeElement(batchEntity);

          this->_model->sendEntityToComponent(entity, this->getNextComponents()->frontConnection(), 0.0);
      }
    } else {
      /* Duplicate the existing entity */
        traceManager->trace(Util::TraceLevel::blockInternal, "Duplicate the existing entity");


        Entity* duplicateEntity;

        _model->getTraceManager()->trace(Util::TraceLevel::blockInternal, "Sending the original entity forward to the first connection");
        this->_model->sendEntityToComponent(entity, this->getNextComponents()->frontConnection(), 0.0);

        for (int k = 0; k < replicationsNumber; k++) {
            /* Duplicate input entity in N entities*/

            if (entity->getEntityTypeName().find("_Batch") != std::string::npos) {
                assert(group != nullptr);
                duplicateEntity = new Entity(elementManager);
                duplicateEntity->setEntityType(entity->getEntityType());

                Entity* newEntity;

                std::string dupBatchEntityID = batchEntityName + batchesCountStr + attributeValue + std::to_string(copyId + 1);

                EntityGroup* duplicateGroup = new EntityGroup(elementManager, dupBatchEntityID);
                elementManager->insert(Util::TypeOf<EntityGroup>(), duplicateGroup);

                List<Entity*>* entitiesToDup = group->getList();

                for (int i = 0; i < entitiesToDup->size(); i++) {
                    Entity* original = entitiesToDup->getAtRank(i);

                    newEntity = new Entity(elementManager);
                    newEntity->setEntityType(original->getEntityType());

                    duplicateGroup->insertElement(newEntity);
                }

                copyId++;

                duplicateEntity->setAttributeValue("Entity.GroupRankID", (double) elementManager->getRankOf(Util::TypeOf<EntityGroup>(), dupBatchEntityID));
                duplicateEntity->setAttributeValue("Entity.Permanent", (bool)entity->getAttributeValue("Entity.Permanent"));
                duplicateEntity->setAttributeValue("Entity.CopyNumber", copyId);
                duplicateEntity->setAttributeValue("Entity.AttributeValue", entity->getAttributeValue("Entity.AttributeValue"));

                elementManager->insert(Util::TypeOf<Entity>(), duplicateEntity);
                _model->getTraceManager()->trace(Util::TraceLevel::blockInternal, "Sending a copied entity forward to the second connection");
                _model->sendEntityToComponent(duplicateEntity, this->getNextComponents()->getConnectionAtRank(1), 0.0);
            } else {
                duplicateEntity = new Entity(elementManager);
                duplicateEntity->setEntityType(entity->getEntityType());
                ElementManager* elements = _model->getElementManager();
                for(int i =0; i<elements->getNumberOfElements(Util::TypeOf<Attribute>());i++){
                    List<ModelElement*>* variaveis = elements->getElements(Util::TypeOf<Attribute>());
                    for(int j =0; j< variaveis->size();j++){
                        Attribute * attr = (Attribute *) variaveis->getAtRank(j);
                        duplicateEntity->setAttributeValue(attr->getName(),entity->getAttributeValue(attr->getName()));
                        
                    }
                }
                _model->getTraceManager()->trace(Util::TraceLevel::blockInternal, "Sending a copied entity forward to the second connection");
                _model->sendEntityToComponent(duplicateEntity, this->getNextComponents()->getConnectionAtRank(1), 0.0);
            }
        }
    }
}

bool Separate::_loadInstance(std::map<std::string, std::string>* fields) {
    bool res = ModelComponent::_loadInstance(fields);
    if (res) {
        //...
    }
    return res;
}

void Separate::_initBetweenReplications() {
}

std::map<std::string, std::string>* Separate::_saveInstance() {
    std::map<std::string, std::string>* fields = ModelComponent::_saveInstance();
    //...
    return fields;
}

bool Separate::_check(std::string* errorMessage) {
    bool resultAll = true;
    //...
    return resultAll;
}

PluginInformation* Separate::GetPluginInformation(){
    PluginInformation* info = new PluginInformation(Util::TypeOf<Separate>(), &Separate::LoadInstance);
    // ...
    return info;
}

void Separate::setAttributeType(bool value){
    this->_attributeType = value;
}