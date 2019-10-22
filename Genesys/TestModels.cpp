/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "TestModels.h"

// GEnSyS Simulator
#include "Simulator.h"

// Model Components
#include "Create.h"
#include "Delay.h"
#include "Dispose.h"
#include "Hold.h"
#include "Signal.h"
#include "Station.h"
#include "Enter.h"
#include "Route.h"
#include "Dummy.h"

// Model elements
#include "EntityType.h"
#include "Attribute.h"
#include "Variable.h"
#include "ProbDistrib.h"
#include "EntityGroup.h"
#include "Formula.h"

TestModels::TestModels() {

}

/**
 * This is the main function of the application. 
 * It instanciates the simulator, builds a simulation model and then simulate that model.
 */
int TestModels::main(int argc, char** argv) {
    Simulator* simulator = new Simulator();

    // insert "fake plugins" since plugins based on dynamic loaded library are not implemented yet
    this->insertFakePluginsByHand(simulator);

    // creates an empty model
    Model* model = new Model(simulator);

    // Handle traces and simulation events to output them
    TraceManager* tm = model->getTraceManager();
    this->setDefaultTraceHandlers(tm);

    // get easy access to classes used to insert components and elements into a model
    ComponentManager* components = model->getComponentManager();
    ElementManager* elements = model->getElementManager();

    //
    // build the simulation model
    //
    ModelInfo* infos = model->getInfos();
    infos->setAnalystName("Your name");
    infos->setProjectTitle("The title of the project");
    infos->setDescription("This simulation model tests the components and elements that have been implemented so far.");
    infos->setReplicationLength(1e3);
    infos->setReplicationLengthTimeUnit(Util::TimeUnit::minute);
    infos->setNumberOfReplications(10);

    EntityType* entityType1 = new EntityType(elements, "Representative_EntityType");
    elements->insert(Util::TypeOf<EntityType>(), entityType1);

    Create* create1 = new Create(model);
    create1->setEntityType(entityType1);
    create1->setTimeBetweenCreationsExpression("40");
    create1->setTimeUnit(Util::TimeUnit::minute);
    create1->setEntitiesPerCreation(1);
    components->insert(create1);

    Delay* delay1 = new Delay(model);
    delay1->setDelayExpression("20");
    delay1->setDelayTimeUnit(Util::TimeUnit::minute);
    components->insert(delay1);
    
    Queue* filaHold1 = new Queue(elements);
    filaHold1->setOrderRule(Queue::OrderRule::FIFO);
    filaHold1->setName("fila1");
    elements->insert(Util::TypeOf<Queue>(), filaHold1);

    Queue* filasignal = new Queue(elements);
    filasignal->setOrderRule(Queue::OrderRule::FIFO);
    filasignal->setName("fila2");
    elements->insert(Util::TypeOf<Queue>(), filasignal);

    Hold* hold1 = new Hold(model);
    hold1->setType(Hold::HoldType::WaitForSignal);
    hold1->setWaitForValueExpr("1");
    hold1->setQueue(filaHold1);    
    components->insert(hold1);
/*
    Station* station1 = new Station(elements);
    station1->setEnterIntoStationComponent(hold1);
    elements->insert(Util::TypeOf<Station>(), station1);

    Enter* enter1 = new Enter(model);
    enter1->setStation(station1);
    components->insert(enter1); 

    Route* route1 = new Route(model);
    route1->setRouteDestinationType(Route::DestinationType::Station);
    route1->setStation(station1);
    components->insert(route1);
*/

    Create* create2 = new Create(model);
    create2->setEntityType(entityType1);
    create2->setTimeBetweenCreationsExpression("30");
    create2->setTimeUnit(Util::TimeUnit::minute);
    create2->setEntitiesPerCreation(1);
    components->insert(create2);

    Signal* signal1 = new Signal(model);
    signal1->setSignalName("1");
    signal1->setQueueName("fila2");
    signal1->setName("1");
    components->insert(signal1);

    Dispose* dispose1 = new Dispose(model);
    components->insert(dispose1);

    Dispose* dispose2 = new Dispose(model);
    components->insert(dispose2);

    // connect model components to create a "workflow" -- should always start from a SourceModelComponent and end at a SinkModelComponent (it will be checked)
    create1->getNextComponents()->insert(delay1);
    delay1->getNextComponents()->insert(hold1);
    hold1->getNextComponents()->insert(dispose1);

    create2->getNextComponents()->insert(signal1);
    signal1->getNextComponents()->insert(dispose2);
    
    // insert the model into the simulator 
    simulator->getModelManager()->insert(model);

    // if the model is ok then save the model into a text file 
    if (model->checkModel()) {
	model->saveModel("./temp/testmodel.txt");
    }

    // execute the simulation
    model->getSimulation()->startSimulation();

    return 0;
};
