/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TestMore.cpp
 * Author: gstvob
 * 
 * Created on 24 de Setembro de 2019, 16:43
 */

#include "TestMore.h"

// you have to included need libs

// GEnSyS Simulator
#include "Simulator.h"

// Model Components
#include "Create.h"
#include "Hold.h"
#include "Delay.h"
#include "Signal.h"
#include "Dispose.h"
#include "Queue.h"
#include "Seize.h"
#include "Release.h"
// Model elements
#include "EntityType.h"
#include "Resource.h"

TestMore::TestMore() {
}

/**
 * This is the main function of the application. 
 * It instanciates the simulator, builds a simulation model and then simulate that model.
 */
int TestMore::main(int argc, char** argv) {
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

    Resource* resource1 = new Resource(elements, "test_res");
    resource1->setCapacity(1);
    elements->insert(Util::TypeOf<Resource>(), resource1);

    //
    Create* create1 = new Create(model);
    create1->setEntityType(entity);
    create1->setTimeBetweenCreationsExpression("10");
    create1->setTimeUnit(Util::TimeUnit::minute);
    create1->setEntitiesPerCreation(1);
    components->insert(create1);
    //
    Create* create2 = new Create(model);
    create2->setEntityType(entity);
    create2->setTimeBetweenCreationsExpression("15");
    create2->setTimeUnit(Util::TimeUnit::minute);
    create2->setEntitiesPerCreation(1);
    components->insert(create2);

    Queue* holdQueue = new Queue(elements, "Queue_HOLD_1");
    holdQueue->setOrderRule(Queue::OrderRule::FIFO);
    elements->insert(Util::TypeOf<Queue>(), holdQueue);

    Queue* seizeQueue = new Queue(elements, "Queue_SEIZE_1");
    seizeQueue->setOrderRule(Queue::OrderRule::FIFO);
    elements->insert(Util::TypeOf<Queue>(), seizeQueue);

    Seize* seize1 = new Seize(model);
    seize1->setPriority(2);
    seize1->setQuantity("1");
    seize1->setResource(resource1);
    seize1->setQueue(seizeQueue);
    components->insert(seize1);
    //
    Hold* hold1 = new Hold(model);
    hold1->setType(Hold::Type::WaitForSignal);
    hold1->setWaitForValueExpr("signal");
    hold1->setQueueName("Queue_HOLD_1");
    components->insert(hold1);
    //
    Dispose* dispose1 = new Dispose(model);
    components->insert(dispose1);
    //
    Delay* delay1 = new Delay(model);
    delay1->setDelayExpression("25");
    delay1->setDelayTimeUnit(Util::TimeUnit::minute);
    components->insert(delay1);

    Release * release1 = new Release(model);
    release1->setQuantity("1");
    release1->setResource(resource1);
    components->insert(release1);
    //
    Signal* signal1 = new Signal(model);
    signal1->setSignalName("signal");
    components->insert(signal1);
    //
    Dispose* dispose2 = new Dispose(model);
    components->insert(dispose2);
    // connect model components to create a "workflow"
    create1->getNextComponents()->insert(seize1);
    seize1->getNextComponents()->insert(hold1);
    hold1->getNextComponents()->insert(delay1);
    delay1->getNextComponents()->insert(release1);
    release1->getNextComponents()->insert(dispose1);
    
    create2->getNextComponents()->insert(signal1);
    signal1->getNextComponents()->insert(dispose2);
    // insert the model into the simulator 
    simulator->getModelManager()->insert(model);

    // if the model is ok then save the model into a text file 
    if (model->checkModel()) {
	model->saveModel("./temp/TestMore.txt");
    }

    // execute the simulation
    model->getSimulation()->startSimulation();

    return 0;
};
