/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TestSignal.cpp
 * Author: rlcancian
 * 
 * Created on 24 de Setembro de 2019, 16:43
 */

#include "TestWhile.h"

// you have to included need libs

// GEnSyS Simulator
#include "Simulator.h"

// Model Components
#include "Create.h"
#include "Hold.h"
#include "Delay.h"
#include "While.h"
#include "Endwhile.h"
#include "Assign.h"
#include "Signal.h"
#include "Dispose.h"
#include "Queue.h"

// Model elements
#include "EntityType.h"
#include "Variable.h"


TestWhile::TestWhile() {
}

/**
 * This is the main function of the application. 
 * It instanciates the simulator, builds a simulation model and then simulate that model.
 */
int TestWhile::main(int argc, char** argv) {
    Simulator* simulator = new Simulator();
    // Handle traces and simulation events to output them
    TraceManager* tm = simulator->getTraceManager();
    this->setDefaultTraceHandlers(tm);
    tm->setTraceLevel(Util::TraceLevel::mostDetailed);
    this->insertFakePluginsByHand(simulator);
    Model* model = new Model(simulator);
    // get easy access to classes used to insert components and elements into a model
    ComponentManager* components = model->getComponentManager();
    ElementManager* elements = model->getElementManager();
    //
    // build the simulation model
    //
    // buildModelWithAllImplementedComponents
    ModelInfo* infos = model->getInfos();
    infos->setReplicationLength(20);
    infos->setReplicationLengthTimeUnit(Util::TimeUnit::hour);
    infos->setNumberOfReplications(3);
    //
    EntityType* entity = new EntityType(elements, "Entity");
    elements->insert(Util::TypeOf<EntityType>(), entity);

    Variable * variable_teste = new Variable("test");
    elements->insert(Util::TypeOf<Variable>(), variable_teste);

    //
    Create* create1 = new Create(model);
    create1->setEntityType(entity);
    create1->setTimeBetweenCreationsExpression("20");
    create1->setTimeUnit(Util::TimeUnit::minute);
    create1->setEntitiesPerCreation(1);
    components->insert(create1);
    //
    Assign* assign1 = new Assign(model);
    Assign::Assignment* assign1Assignment = new Assign::Assignment(Assign::DestinationType::Variable, "teste", "0");
    assign1->getAssignments()->insert(assign1Assignment);
    components->insert(assign1);

    While* while2 = new While(model);
    Endwhile* endwhile2 = new Endwhile(model);
    while2->set_condition("teste <= 10");
    while2->attach_endwhile(endwhile2);
    endwhile2->attach_while(while2);
    components->insert(while2);
    components->insert(endwhile2);
    //

    Delay* delay1 = new Delay(model);
    delay1->setDelayExpression("15");
    delay1->setDelayTimeUnit(Util::TimeUnit::second);
    components->insert(delay1);

    //
    Assign* assign2 = new Assign(model);
    Assign::Assignment* assign2Assignment = new Assign::Assignment(Assign::DestinationType::Variable, "teste", "teste+1");
    assign2->getAssignments()->insert(assign2Assignment);
    components->insert(assign2);
    //
    Dispose* dispose1 = new Dispose(model);
    components->insert(dispose1);
    //
    // connect model components to create a "workflow"
    create1->getNextComponents()->insert(assign1);
    assign1->getNextComponents()->insert(while2);

    while2->getNextComponents()->insert(delay1);
    delay1->getNextComponents()->insert(assign2);
    assign2->getNextComponents()->insert(endwhile2);
    endwhile2->getNextComponents()->insert(dispose1);
    // insert the model into the simulator 
    simulator->getModelManager()->insert(model);

    // if the model is ok then save the model into a text file 
    if (model->checkModel()) {
	model->saveModel("./temp/testwhile.txt");
    }

    // execute the simulation
    model->getSimulation()->startSimulation();

    return 0;
};
