/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Hold.h
 * Author: gstvob
 *
 * Created on 03 de Junho de 2019, 15:20
 */

#ifndef HOLD_H
#define HOLD_H

#include "ModelComponent.h"
#include "Plugin.h"
#include "Queue.h"

class Hold : public ModelComponent {

public:
    enum class HoldType: int {
        WaitForSignal=0,
        ScanForCondition=1,
        InfiniteHold=2
    };
    Hold(Model* model);
    Hold(const Hold& orig);
    virtual ~Hold();

    virtual std::string show();
    static PluginInformation* GetPluginInformation();
    static ModelComponent* LoadInstance(Model* model, std::map<std::string, std::string>* fields);

    void setWaitForValueExpr(std::string _expr);
    void setType(HoldType _type);
    void setQueue(Queue* _name) throw();

    HoldType getType() const;
    std::string getWaitForValueExpr() const;
    std::string getQueueName() const;

    void release_signal(int _limit);

protected:
    virtual void _execute(Entity* entity);
    virtual bool _loadInstance(std::map<std::string, std::string>* fields);
    virtual void _initBetweenReplications();
    virtual std::map<std::string, std::string>* _saveInstance();
    virtual bool _check(std::string* errorMessage);

private:
    std::string _wait_for_value = "1";
    HoldType _type = HoldType::WaitForSignal;
    Queue* _queue;
};

#endif /* HOLD_H */

