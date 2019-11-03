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

#include "PP4Model1.h"

// you have to included need libs

// GEnSyS Simulator
#include "Simulator.h"

// Model Components
#include "Create.h"
#include "Assign.h"
#include "Route.h"
#include "Enter.h"
#include "Decide.h"
#include "Separate.h"
#include "Hold.h"
#include "Delay.h"
#include "Signal.h"
#include "Dispose.h"
#include "Queue.h"

// Model elements
#include "EntityType.h"
#include "Attribute.h"
#include "Variable.h"
#include "Resource.h"
#include "Station.h"


PP4Model1::PP4Model1() {
}

/**
 * This is the main function of the application. 
 * It instanciates the simulator, builds a simulation model and then simulate that model.
 */
int PP4Model1::main(int argc, char** argv) {
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
    infos->setReplicationLength(60);
    infos->setReplicationLengthTimeUnit(Util::TimeUnit::second);
    infos->setNumberOfReplications(3);
    
    //Entities
    EntityType* process = new EntityType(elements, "Process");
    elements->insert(Util::TypeOf<EntityType>(), process);
    EntityType* init = new EntityType(elements, "Init");
    elements->insert(Util::TypeOf<EntityType>(), init);
    EntityType* memoryAccess = new EntityType(elements, "Memory Access");
    elements->insert(Util::TypeOf<EntityType>(), memoryAccess);

    Attribute* pid = new Attribute("pid");
    elements->insert(Util::TypeOf<Attribute>(), pid);
    Attribute* pageNumber = new Attribute("PageNumber");
    elements->insert(Util::TypeOf<Attribute>(), pageNumber);
    Attribute* physAddr = new Attribute("PhysAddr");
    elements->insert(Util::TypeOf<Attribute>(), physAddr);

    Queue* seizeMMU = new Queue(elements, "SeizeMMU.Queue");
    seizeMMU->setOrderRule(Queue::OrderRule::FIFO);
    elements->insert(Util::TypeOf<Queue>(), seizeMMU);
    Queue* holdSuspended = new Queue(elements, "HoldSuspended.Queue");
    holdSuspended->setOrderRule(Queue::OrderRule::FIFO);
    elements->insert(Util::TypeOf<Queue>(), holdSuspended);
    Queue* holdProcessMMU = new Queue(elements, "HoldProcessMMU.Queue");
    holdProcessMMU->setOrderRule(Queue::OrderRule::FIFO);
    elements->insert(Util::TypeOf<Queue>(), holdProcessMMU);
    
    Resource* mmu = new Resource(elements, "MMU");
    mmu->setCapacity(1);
    elements->insert(Util::TypeOf<Resource>(), mmu);
    //later
    Variable* var1 = new Variable("varNextIndex");
    elements->insert(Util::TypeOf<Variable>(), var1);

    /*
        ADVANCED TEST
    */

    /*
        Expression
    */

    /*
        STATISTICS
    */
    //


    /*
        Process Creation
    */
    Create* createInits = new Create(model);
    createInits->setEntityType(init);
    createInits->setTimeBetweenCreationsExpression("1");
    createInits->setTimeUnit(Util::TimeUnit::second);
    createInits->setEntitiesPerCreation(2); // VAR Process.MAXCOUNT
    components->insert(createInits);

    Assign* startProcess = new Assign(model);
    startProcess->getAssignments()->insert(Assign::Assignment(Assign:));// ASSIGN EntityType, Process
    components->insert(startProcess);

    Station* processLoopStation = new Station(elements, "ProcLoop");
    elements->insert(Util::TypeOf<Station>(), processLoopStation);

    Route* procLoop = new Route(model);
    procLoop->setRouteDestinationType(Route::DestinationType::Station);
    procLoop->setRouteTimeExpression(0);
    procLoop->setRouteTimeTimeUnit(Util::TimeUnit::hour);
    procLoop->setStation(processLoopStation);//ProcLoop station
    components->insert(procLoop);

    createInits->getNextComponents()->insert(startProcess);
    startProcess->getNextComponents()->insert(procLoop);

    /*
        Process Loop
    */

    Enter* processLoop = new Enter(model);
    processLoop->setStation(processLoopStation);
    components->insert(processLoop);
    
    Delay* cpuComputing = new Delay(model);
    cpuComputing->setDelayExpression("tria(3, 8.96, 12) * 0.000000001");
    cpuComputing->setDelayTimeUnit(Util::TimeUnit::second);
    components->insert(cpuComputing);
    
    Decide* accessUltimaPagina = new Decide(model);
    accessUltimaPagina->getConditions()->insert("NQ(Queue_Machine_1) <= 2*NQ(Queue_Machine_2)");//2 way by chance
    
    Separate* accessMemory = new Separate(model);
    accessMemory->setAmountToDuplicate("1");

    Assign* pegarPaginaAleatoria = new Assign(model);
    pegarPaginaAleatoria->getAssignments()->insert(Assign::Assignment(Assign::DestinationType::Attribute,"PageNumber","ROUND(UNIF(1, PageCount))"));
    
    Hold* hold1 = new Hold(model);
    hold1->setType(Hold::Type::WaitForSignal);
    hold1->setWaitForValueExpr("pid");
    hold1->setQueueName("HoldProcessMMU.Queue");
    components->insert(hold1);
    
    //
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
    delay1->setDelayExpression("norm(3,1)");
    delay1->setDelayTimeUnit(Util::TimeUnit::second);
    components->insert(delay1);
    //
    Queue* signalQueue = new Queue(elements, "signalQueue");
    signalQueue->setOrderRule(Queue::OrderRule::FIFO);
    elements->insert(Util::TypeOf<Queue>(), signalQueue);
    //
    Signal* signal1 = new Signal(model);
    signal1->setSignalName("signal");
    signal1->setQueueName("signalQueue");
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
	model->saveModel("./temp/PP4Model1.txt");
    }

    // execute the simulation
    model->getSimulation()->startSimulation();

    return 0;
};
