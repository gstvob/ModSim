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
#include "Seize.h"
#include "Search.h"
#include "Hold.h"
#include "While.h"
#include "Endwhile.h"
#include "Release.h"
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

    Queue* seizeMMUQ = new Queue(elements, "SeizeMMU.Queue");
    seizeMMUQ->setOrderRule(Queue::OrderRule::FIFO);
    elements->insert(Util::TypeOf<Queue>(), seizeMMUQ);
    Queue* holdSuspended = new Queue(elements, "HoldSuspended.Queue");
    holdSuspended->setOrderRule(Queue::OrderRule::FIFO);
    elements->insert(Util::TypeOf<Queue>(), holdSuspended);
    Queue* holdProcessMMU = new Queue(elements, "HoldProcessMMU.Queue");
    holdProcessMMU->setOrderRule(Queue::OrderRule::FIFO);
    elements->insert(Util::TypeOf<Queue>(), holdProcessMMU);
    
    Resource* mmu = new Resource(elements, "MMU");
    mmu->setCapacity(1);
    elements->insert(Util::TypeOf<Resource>(), mmu);

    Variable* memorySize = new Variable("MemorySize");
    Variable* pageSize = new Variable("PageSize");
    Variable* processCount = new Variable("ProcessCount");
    Variable* processMaxCount = new Variable("ProcessMaxCount");
    Variable* tlbVar = new Variable("TLB"); // 6rows, 2 columns
    Variable* diskTransferRate = new Variable("DiskTransferRate");
    Variable* TLBi = new Variable("TLBi");
    Variable* TLBmin = new Variable("TLBmin");
    Variable* pageTables = new Variable("PageTables");
    Variable* mainMemory = new Variable("MainMemory");
    Variable* memToRemove = new Variable("MemToRemove");
    Variable* MEMi = new Variable("MEMi");
    Variable* MEMusage = new Variable("MEMusage");
    Variable* breakLoop = new Variable("BreakLoop");
    Variable* suspend = new Variable("Suspend");
    Variable* tlbHits = new Variable("TLBhits");
    elements->insert(Util::TypeOf<Variable>(), memorySize);
    elements->insert(Util::TypeOf<Variable>(), pageSize);
    elements->insert(Util::TypeOf<Variable>(), processCount);
    elements->insert(Util::TypeOf<Variable>(), processMaxCount);
    elements->insert(Util::TypeOf<Variable>(), tlbVar);
    elements->insert(Util::TypeOf<Variable>(), diskTransferRate);
    elements->insert(Util::TypeOf<Variable>(), TLBi);
    elements->insert(Util::TypeOf<Variable>(), TLBmin);
    elements->insert(Util::TypeOf<Variable>(), pageTables);
    elements->insert(Util::TypeOf<Variable>(), mainMemory);
    elements->insert(Util::TypeOf<Variable>(), memToRemove);
    elements->insert(Util::TypeOf<Variable>(), MEMi);
    elements->insert(Util::TypeOf<Variable>(), MEMusage);
    elements->insert(Util::TypeOf<Variable>(), breakLoop);
    elements->insert(Util::TypeOf<Variable>(), suspend);
    elements->insert(Util::TypeOf<Variable>(), tlbHits);


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
    Assign::Assignment* switch_entity = new Assign::Assignment(process);
    Assign::Assignment* processCountInc = new Assign::Assignment(Assign::DestinationType::Variable, "ProcessCount", "ProcessCount + 1"); 
    Assign::Assignment* pidAtt = new Assign::Assignment(Assign::DestinationType::Attribute, "pid", "ProcessCount");
    Assign::Assignment* pageNumberAtt = new Assign::Assignment(Assign::DestinationType::Attribute, "PageNumber", "AINT(UNIF(1, PageCount))"); 
    startProcess->getAssignments()->insert(switch_entity);
    startProcess->getAssignments()->insert(processCountInc);
    startProcess->getAssignments()->insert(pidAtt);
    startProcess->getAssignments()->insert(pageNumberAtt);
    components->insert(startProcess);

    Station* processLoopStation = new Station(elements, "ProcLoop");
    elements->insert(Util::TypeOf<Station>(), processLoopStation);

    Route* procLoop = new Route(model);
    procLoop->setRouteDestinationType(Route::DestinationType::Station);
    procLoop->setRouteTimeExpression(0);
    procLoop->setRouteTimeTimeUnit(Util::TimeUnit::hour);
    procLoop->setStation(processLoopStation);
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
    accessUltimaPagina->getConditions()->insert("UNIF(0,1) >= 0.98");//2 way by chance
    components->insert(accessUltimaPagina);

    Separate* accessMemory = new Separate(model);
    accessMemory->setAmountToDuplicate("1");
    components->insert(accessMemory);

    Assign* pegarPaginaAleatoria = new Assign(model);
    Assign::Assignment* pegarPaginaAleatoriaAssignment = new Assign::Assignment(Assign::DestinationType::Attribute, "PageNumber", "ROUND(UNIF(1, PageCount))");
    pegarPaginaAleatoria->getAssignments()->insert(pegarPaginaAleatoriaAssignment);
    components->insert(pegarPaginaAleatoria);

    Hold* hold1 = new Hold(model);
    hold1->setType(Hold::Type::WaitForSignal);
    hold1->setWaitForValueExpr("pid");
    hold1->setQueueName("HoldProcessMMU.Queue");
    components->insert(hold1);

    Assign* assignAccessType = new Assign(model);
    Assign::Assignment* assignAccessTypeAssignment = new Assign::Assignment(memoryAccess);
    assignAccessType->getAssignments()->insert(assignAccessTypeAssignment);
    components->insert(assignAccessType);

    Station* startMemAccessStation = new Station(elements, "StartMemAccessStation");
    elements->insert(Util::TypeOf<Station>(), startMemAccessStation);

    Route* startAccess = new Route(model);
    startAccess->setRouteDestinationType(Route::DestinationType::Station);
    startAccess->setRouteTimeExpression(0);
    startAccess->setRouteTimeTimeUnit(Util::TimeUnit::hour);
    startAccess->setStation(startMemAccessStation);
    components->insert(startAccess);

    Decide* decideSuspend = new Decide(model);
    decideSuspend->getConditions()->insert("suspend == 1");//VAR suspend == 1
    components->insert(decideSuspend);
    
    Assign* resetSuspend = new Assign(model);
    Assign::Assignment* resetSuspendAssignment = new Assign::Assignment(Assign::DestinationType::Variable, "Suspend", "0");
    resetSuspend->getAssignments()->insert(resetSuspendAssignment);
    components->insert(resetSuspend); 

    Hold* hold2 = new Hold(model);
    hold2->setType(Hold::Type::WaitForSignal);
    hold2->setWaitForValueExpr("pid");
    hold2->setQueueName("HoldSuspended.Queue");
    components->insert(hold2);

    processLoop->getNextComponents()->insert(cpuComputing);
    cpuComputing->getNextComponents()->insert(accessUltimaPagina);
    accessUltimaPagina->getNextComponents()->insert(accessMemory);
    accessUltimaPagina->getNextComponents()->insert(pegarPaginaAleatoria);
    pegarPaginaAleatoria->getNextComponents()->insert(accessMemory);
    accessMemory->getNextComponents()->insert(hold1);
    accessMemory->getNextComponents()->insert(assignAccessType);
    assignAccessType->getNextComponents()->insert(startAccess);
    hold1->getNextComponents()->insert(decideSuspend);
    decideSuspend->getNextComponents()->insert(processLoop);
    decideSuspend->getNextComponents()->insert(resetSuspend);
    resetSuspend->getNextComponents()->insert(hold2);
    hold2->getNextComponents()->insert(hold1);
    

    /*
        Acesso a memoria
    */
    Enter* startMemoryAccess = new Enter(model);
    startMemoryAccess->setStation(startMemAccessStation);
    components->insert(startMemoryAccess);

    Seize* seizeMMU = new Seize(model);
    seizeMMU->setQueue(seizeMMUQ);
    seizeMMU->setPriority(2);
    seizeMMU->setResource(mmu);
    components->insert(seizeMMU);

    Delay* delayTLB = new Delay(model);
    delayTLB->setDelayExpression("0.000000001");
    delayTLB->setDelayTimeUnit(Util::TimeUnit::second);
    components->insert(delayTLB);

    Search* searchTLB = new Search(model);
/*    searchTLB->setType(2);
    searchTLB->setSearchCondition("TLB[J, pid] == PageNumber");
    searchTLB->setStartingRank("1");
    searchTLB->setEndingRank("TLB.size");*/
    components->insert(searchTLB);

    Delay* delayRAM = new Delay(model);
    delayRAM->setDelayExpression("0.00000001");
    delayRAM->setDelayTimeUnit(Util::TimeUnit::second);
    components->insert(delayRAM);

    Delay* delayPageTableAccess = new Delay(model);
    delayPageTableAccess->setDelayExpression("0.00000001");
    delayPageTableAccess->setDelayTimeUnit(Util::TimeUnit::second);
    components->insert(delayPageTableAccess);

    Station* endMemAccessStation = new Station(elements);
    elements->insert(Util::TypeOf<Station>(), endMemAccessStation);

    Route* routeEndAccess = new Route(model);
    routeEndAccess->setRouteDestinationType(Route::DestinationType::Station);
    routeEndAccess->setRouteTimeExpression(0);
    routeEndAccess->setRouteTimeTimeUnit(Util::TimeUnit::hour);
    routeEndAccess->setStation(endMemAccessStation);
    components->insert(routeEndAccess);

    Assign* assignPhysAddr = new Assign(model);
    Assign::Assignment* assignPhysAddrAssignment = new Assign::Assignment(Assign::DestinationType::Attribute, "PhysAddr", "PageTables[pid*PageNumber, 2]");
    assignPhysAddr->getAssignments()->insert(assignPhysAddrAssignment);
    components->insert(assignPhysAddr);

    Decide* checkPageTable = new Decide(model);
    checkPageTable->getConditions()->insert("PhysAddr != 0");//PhysAddr != 0
    components->insert(checkPageTable);

    Station* updateTLBStation = new Station(elements);
    elements->insert(Util::TypeOf<Station>(), updateTLBStation);
    Station* updateMEMStation = new Station(elements);
    elements->insert(Util::TypeOf<Station>(), updateMEMStation);

    Route* routeUpdateTLB = new Route(model);
    routeUpdateTLB->setRouteDestinationType(Route::DestinationType::Station);
    routeUpdateTLB->setRouteTimeExpression(0);
    routeUpdateTLB->setRouteTimeTimeUnit(Util::TimeUnit::hour);
    routeUpdateTLB->setStation(updateTLBStation);
    components->insert(routeUpdateTLB);
    
    Route* routeUpdateMEM = new Route(model);
    routeUpdateMEM->setRouteDestinationType(Route::DestinationType::Station);
    routeUpdateMEM->setRouteTimeExpression(0);
    routeUpdateMEM->setRouteTimeTimeUnit(Util::TimeUnit::hour);
    routeUpdateMEM->setStation(updateMEMStation);
    components->insert(routeUpdateMEM);

    startMemoryAccess->getNextComponents()->insert(seizeMMU);
    seizeMMU->getNextComponents()->insert(delayTLB);
    delayTLB->getNextComponents()->insert(searchTLB);
    searchTLB->getNextComponents()->insert(delayRAM);
    searchTLB->getNextComponents()->insert(delayPageTableAccess);
    delayRAM->getNextComponents()->insert(routeEndAccess);
    delayPageTableAccess->getNextComponents()->insert(assignPhysAddr);
    assignPhysAddr->getNextComponents()->insert(checkPageTable);
    checkPageTable->getNextComponents()->insert(routeUpdateTLB);
    checkPageTable->getNextComponents()->insert(routeUpdateMEM);


    /*
        Fim do Acesso.
    */

    Enter* endAccess = new Enter(model);
    endAccess->setStation(endMemAccessStation);
    components->insert(endAccess);

    Assign* updatePageTable = new Assign(model);
    Assign::Assignment* updatePageTableAssignment = new Assign::Assignment(Assign::DestinationType::Variable, "PageTables", "PageTables[pid*PageNumber,1]+1", "pid * PageNumber","1");
    updatePageTable->getAssignments()->insert(updatePageTableAssignment);
    components->insert(updatePageTable);
    
    Release* releaseMMU = new Release(model);
    releaseMMU->setResource(mmu);
    releaseMMU->setQuantity("1");

    Signal* signalProcess = new Signal(model);
    signalProcess->setSignalName("pid");
    components->insert(signalProcess);

    Dispose* disposeMEMAccess = new Dispose(model);
    components->insert(disposeMEMAccess);

    endAccess->getNextComponents()->insert(updatePageTable);
    updatePageTable->getNextComponents()->insert(releaseMMU);
    releaseMMU->getNextComponents()->insert(signalProcess);
    signalProcess->getNextComponents()->insert(disposeMEMAccess);

    /*
        Update TLB
    */

    Enter* updateTLB = new Enter(model);
    updateTLB->setStation(updateTLBStation);
    components->insert(updateTLB);

    Assign* initLoop = new Assign(model);
    Assign::Assignment* initLoopAssignment1 = new Assign::Assignment(Assign::DestinationType::Variable, "TLBi", "1");
    Assign::Assignment* initLoopAssignment2 = new Assign::Assignment(Assign::DestinationType::Variable, "TLBmin", "1");
    Assign::Assignment* initLoopAssignment3 = new Assign::Assignment(Assign::DestinationType::Variable, "BreakLoop", "0");

    initLoop->getAssignments()->insert(initLoopAssignment1);
    initLoop->getAssignments()->insert(initLoopAssignment2);
    initLoop->getAssignments()->insert(initLoopAssignment3);
    components->insert(initLoop);

    /*
        while blocks
    */


    While* while1 = new While(model);
    Endwhile* endwhile1 = new Endwhile(model);

    while1->set_condition("(TLBi <= TLBsize) && (BreakLoop == 0)");
    while1->attach_endwhile(endwhile1);
    endwhile1->attach_while(while1);
    components->insert(while1);
    components->insert(endwhile1);

    Decide * tlbRowNotEmpty = new Decide(model);
    tlbRowNotEmpty->getConditions()->insert("TLB[TLBi, pid] != 0");
    components->insert(tlbRowNotEmpty);

    Assign* breakA = new Assign(model);
    Assign::Assignment* breakAssignment = new Assign::Assignment(Assign::DestinationType::Variable, "BreakLoop", "1");
    Assign::Assignment* breakAssignment2 = new Assign::Assignment(Assign::DestinationType::Variable, "TLBi", "TLBmin");
    breakA->getAssignments()->insert(breakAssignment);
    breakA->getAssignments()->insert(breakAssignment2);
    components->insert(breakA);

    Decide * decideMenor = new Decide(model);
    decideMenor->getConditions()->insert("PageTables[pid*TLB[TLBi, pid],1] <  PageTables[pid*TLB[TLBmin, pid],1]"); 
    components->insert(decideMenor);

    Assign* assignMin = new Assign(model);
    Assign::Assignment* assignMinAssignment = new Assign::Assignment(Assign::DestinationType::Variable, "TLBi", "TLBmin");
    assignMin->getAssignments()->insert(assignMinAssignment);
    components->insert(assignMin);

    Assign* incTLBi = new Assign(model);
    Assign::Assignment* incTLBiAssignment = new Assign::Assignment(Assign::DestinationType::Variable, "TLBi", "TLBi+1");
    incTLBi->getAssignments()->insert(incTLBiAssignment);
    components->insert(incTLBi);

    Assign* replaceTLBEntry = new Assign(model);
    Assign::Assignment* replaceTLBEntryAssignment = new Assign::Assignment(Assign::DestinationType::Variable, "BreakLoop", "0");
    Assign::Assignment* replaceTLBEntryAssignment2 = new Assign::Assignment(Assign::DestinationType::Variable, "TLB", "PageNumber", "TLBmin", "pid");
    replaceTLBEntry->getAssignments()->insert(replaceTLBEntryAssignment);
    replaceTLBEntry->getAssignments()->insert(replaceTLBEntryAssignment2);
    components->insert(replaceTLBEntry);


    Delay* delayMainMemAndTLB = new Delay(model);
    delayMainMemAndTLB->setDelayExpression("0.000000011");
    delayMainMemAndTLB->setDelayTimeUnit(Util::TimeUnit::second);
    components->insert(delayMainMemAndTLB);

    Route* routeEnd = new Route(model);
    routeEnd->setStation(endMemAccessStation);
    routeEnd->setRouteTimeExpression("0");
    routeEnd->setRouteTimeTimeUnit(Util::TimeUnit::hour);
    components->insert(routeEnd);


    updateTLB->getNextComponents()->insert(initLoop);
    initLoop->getNextComponents()->insert(while1);
    while1->getNextComponents()->insert(tlbRowNotEmpty);
    tlbRowNotEmpty->getNextComponents()->insert(decideMenor);
    tlbRowNotEmpty->getNextComponents()->insert(breakA);
    breakA->getNextComponents()->insert(endwhile1);
    decideMenor->getNextComponents()->insert(assignMin);
    decideMenor->getNextComponents()->insert(incTLBi);
    assignMin->getNextComponents()->insert(incTLBi);
    incTLBi->getNextComponents()->insert(endwhile1);
    endwhile1->getNextComponents()->insert(replaceTLBEntry);
    replaceTLBEntry->getNextComponents()->insert(delayMainMemAndTLB);
    delayMainMemAndTLB->getNextComponents()->insert(routeEnd);

    /*
        Substituição de paginas
    */

    Enter* updateMEM = new Enter(model);
    updateMEM->setStation(updateMEMStation);
    components->insert(updateMEM);

    Assign* initLoopM = new Assign(model);
    Assign::Assignment* initLoopMAssignment = new Assign::Assignment(Assign::DestinationType::Variable, "MemToRemove", "1");
    Assign::Assignment* initLoopMAssignment2 = new Assign::Assignment(Assign::DestinationType::Variable, "MEMi", "1");
    Assign::Assignment* initLoopMAssignment3 = new Assign::Assignment(Assign::DestinationType::Variable, "BreakLoop", "0");

    initLoopM->getAssignments()->insert(initLoopMAssignment);
    initLoopM->getAssignments()->insert(initLoopMAssignment2);
    initLoopM->getAssignments()->insert(initLoopMAssignment3);

    components->insert(initLoopM);

    While* while2 = new While(model);
    Endwhile* endwhile2 = new Endwhile(model);
    while2->set_condition("(MEMi <= PageCount) && (BreakLoop == 0)");
    while2->attach_endwhile(endwhile2);
    endwhile2->attach_while(while2);
    components->insert(while2);
    components->insert(endwhile2);
    
    Decide* frameBufferNotEmpty = new Decide(model);
    frameBufferNotEmpty->getConditions()->insert("MainMemory[MEMi, 1] != 0");
    components->insert(frameBufferNotEmpty);

    Decide* decideUpdateMemtoremove = new Decide(model);
    decideUpdateMemtoremove->getConditions()->insert("PageTables[MainMemory[MEMi,1]*MainMemory[MEMi,2],1] <  PageTables[MainMemory[MemToRemove,1]*MainMemory[MemToRemove,2],1]");
    components->insert(decideUpdateMemtoremove);

    Assign* memToRemoveA = new Assign(model);
    Assign::Assignment* memToRemoveAssignment = new Assign::Assignment(Assign::DestinationType::Variable, "MemToRemove", "MEMi");
    memToRemoveA->getAssignments()->insert(memToRemoveAssignment);
    components->insert(memToRemoveA);
    
    Assign* memI = new Assign(model);
    Assign::Assignment* memIAssignment = new Assign::Assignment(Assign::DestinationType::Variable, "MEMi", "MEMi+1");
    memI->getAssignments()->insert(memIAssignment);
    components->insert(memI);

    Assign* breakLoopA = new Assign(model);
    Assign::Assignment* breakLoopAssignment = new Assign::Assignment(Assign::DestinationType::Variable, "BreakLoop", "1");
    Assign::Assignment* breakLoopAssignment2 = new Assign::Assignment(Assign::DestinationType::Variable, "MemToRemove", "MEMi");

    breakLoopA->getAssignments()->insert(breakLoopAssignment);
    breakLoopA->getAssignments()->insert(breakLoopAssignment2);

    components->insert(breakLoopA);

    /*
        Algoritmo de sub pt2
    */

    Signal* signalSuspendProcess = new Signal(model);
    signalSuspendProcess->setSignalName("pid");
    components->insert(signalSuspendProcess);

    Assign* assignSuspend = new Assign(model);
    Assign::Assignment* assignSuspendAssignment1 = new Assign::Assignment(Assign::DestinationType::Variable, "BreakLoop", "0");
    Assign::Assignment* assignSuspendAssignment2 = new Assign::Assignment(Assign::DestinationType::Variable, "Suspend", "1");
    assignSuspend->getAssignments()->insert(assignSuspendAssignment1);
    assignSuspend->getAssignments()->insert(assignSuspendAssignment2);
    components->insert(assignSuspend);

    Decide* memToRemoveEmpty = new Decide(model);
    memToRemoveEmpty->getConditions()->insert("MainMemory[MemToRemove,1] == 0");
    components->insert(memToRemoveEmpty);

    Assign* increaseMemUsage = new Assign(model);
    Assign::Assignment* increaseMemUsageAssignment = new Assign::Assignment(Assign::DestinationType::Variable, "MEMusage", "MEMusage+1");
    increaseMemUsage->getAssignments()->insert(increaseMemUsageAssignment);
    components->insert(increaseMemUsage);

    Assign* clearPageTableEntry = new Assign(model);
    Assign::Assignment* clearPageTableEntryAssignment1 = new Assign::Assignment(Assign::DestinationType::Variable, "PageTables[MainMemory[MemToRemove,1]*MainMemory[MemToRemove,2],1]", "0"); // AQUELES VAR LOCO
    Assign::Assignment* clearPageTableEntryAssignment2 = new Assign::Assignment(Assign::DestinationType::Variable, "PageTables[MainMemory[MemToRemove,1]*MainMemory[MemToRemove,2],2]", "0");
    clearPageTableEntry->getAssignments()->insert(clearPageTableEntryAssignment1);
    clearPageTableEntry->getAssignments()->insert(clearPageTableEntryAssignment2);
    components->insert(clearPageTableEntry);

    Decide* decidePageChanged = new Decide(model);
    decidePageChanged->getConditions()->insert("UNIF(0,1) <= 0.1");
    components->insert(decidePageChanged);

    Delay* delaySavePageToDisk = new Delay(model);
    delaySavePageToDisk->setDelayExpression("(PageSize / 1024) * DiskTransferRate * 1"); //mais algumas coisas.
    components->insert(delaySavePageToDisk);

    Assign* replaceFrameBuffer = new Assign(model);
    Assign::Assignment* replaceFrameBufferAssignment1 = new Assign::Assignment(Assign::DestinationType::Variable, "MainMemory", "MemToRemove", "1", "pid");// mais assigns esquisitos.
    Assign::Assignment* replaceFrameBufferAssignment2 = new Assign::Assignment(Assign::DestinationType::Variable, "MainMemory", "MemToRemove", "2", "PageNumber");
    Assign::Assignment* replaceFrameBufferAssignment3 = new Assign::Assignment(Assign::DestinationType::Attribute, "PhysAddr", "MemToRemove");
    Assign::Assignment* replaceFrameBufferAssignment4 = new Assign::Assignment(Assign::DestinationType::Variable, "PageTables", "MemToRemove", "pid*PageNumber", "2");

    replaceFrameBuffer->getAssignments()->insert(replaceFrameBufferAssignment1);
    replaceFrameBuffer->getAssignments()->insert(replaceFrameBufferAssignment2);
    replaceFrameBuffer->getAssignments()->insert(replaceFrameBufferAssignment3);
    replaceFrameBuffer->getAssignments()->insert(replaceFrameBufferAssignment4);

    components->insert(replaceFrameBuffer);

    Delay* delayDisco = new Delay(model);
    delayDisco->setDelayExpression("(PageSize / 1024) * DiskTransferRate * 1"); //mais algumas coisas.
    components->insert(delayDisco);

    Signal* signalResumeProcess = new Signal(model);
    signalResumeProcess->setSignalName("pid");
    components->insert(signalResumeProcess);

    Route * updTLB = new Route (model);
    updTLB->setStation(updateTLBStation);
    updTLB->setRouteTimeExpression("0");
    updTLB->setRouteTimeTimeUnit(Util::TimeUnit::hour);
    components->insert(updTLB);

    updateMEM->getNextComponents()->insert(initLoopM);
    initLoopM->getNextComponents()->insert(while2);
    while2->getNextComponents()->insert(frameBufferNotEmpty);
    frameBufferNotEmpty->getNextComponents()->insert(decideUpdateMemtoremove);
    frameBufferNotEmpty->getNextComponents()->insert(breakLoopA);
    breakLoopA->getNextComponents()->insert(endwhile2);
    decideUpdateMemtoremove->getNextComponents()->insert(memToRemoveA);
    decideUpdateMemtoremove->getNextComponents()->insert(memI);
    memToRemoveA->getNextComponents()->insert(memI);
    memI->getNextComponents()->insert(endwhile2);
    endwhile2->getNextComponents()->insert(signalSuspendProcess);
    signalSuspendProcess->getNextComponents()->insert(assignSuspend);
    assignSuspend->getNextComponents()->insert(memToRemoveEmpty);
    memToRemoveEmpty->getNextComponents()->insert(increaseMemUsage);
    memToRemoveEmpty->getNextComponents()->insert(clearPageTableEntry);
    increaseMemUsage->getNextComponents()->insert(replaceFrameBuffer);
    clearPageTableEntry->getNextComponents()->insert(decidePageChanged);
    decidePageChanged->getNextComponents()->insert(delaySavePageToDisk);
    decidePageChanged->getNextComponents()->insert(replaceFrameBuffer);
    delaySavePageToDisk->getNextComponents()->insert(replaceFrameBuffer);
    replaceFrameBuffer->getNextComponents()->insert(delayDisco);
    delayDisco->getNextComponents()->insert(signalResumeProcess);
    signalResumeProcess->getNextComponents()->insert(routeUpdateTLB);

    // if the model is ok then save the model into a text file 
    if (model->checkModel()) {
    	model->saveModel("./temp/PP4Model1.txt");
    }
    // execute the simulation
    model->getSimulation()->startSimulation();

    return 0;
};
