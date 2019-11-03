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

#include "TestSignal.h"

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

// Model elements
#include "EntityType.h"


TestSignal::TestSignal() {
}

/**
 * This is the main function of the application. 
 * It instanciates the simulator, builds a simulation model and then simulate that model.
 */
int TestSignal::main(int argc, char** argv) {
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
    infos->setReplicationLengthTimeUnit(Util::TimeUnit::second);
    infos->setNumberOfReplications(3);
    //
    EntityType* entity = new EntityType(elements, "Entity");
    elements->insert(Util::TypeOf<EntityType>(), entity);
    //
    Create* create1 = new Create(model);
    create1->setEntityType(entity);
    create1->setTimeBetweenCreationsExpression("3");
    create1->setTimeUnit(Util::TimeUnit::second);
    create1->setEntitiesPerCreation(1);
    components->insert(create1);
    //
    Create* create2 = new Create(model);
    create2->setEntityType(entity);
    create2->setTimeBetweenCreationsExpression("12");
    create2->setTimeUnit(Util::TimeUnit::second);
    create2->setEntitiesPerCreation(1);
    components->insert(create2);

    Queue* holdQueue = new Queue(elements, "Queue_HOLD_1");
    holdQueue->setOrderRule(Queue::OrderRule::FIFO);
    elements->insert(Util::TypeOf<Queue>(), holdQueue);
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
    delay1->setDelayExpression("10");
    delay1->setDelayTimeUnit(Util::TimeUnit::second);
    components->insert(delay1);
    //
    Signal* signal1 = new Signal(model);
    signal1->setSignalName("signal");
    components->insert(signal1);
    //
    Dispose* dispose2 = new Dispose(model);
    components->insert(dispose2);
    // connect model components to create a "workflow"
    create1->getNextComponents()->insert(hold1);
    hold1->getNextComponents()->insert(dispose1);

    create2->getNextComponents()->insert(delay1);
    delay1->getNextComponents()->insert(signal1);
    signal1->getNextComponents()->insert(dispose2);
    // insert the model into the simulator 
    simulator->getModelManager()->insert(model);

    // if the model is ok then save the model into a text file 
    if (model->checkModel()) {
	model->saveModel("./temp/testsignal.txt");
    }

    // execute the simulation
    model->getSimulation()->startSimulation();

    return 0;
};
