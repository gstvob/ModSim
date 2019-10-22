/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   While.h
 * Author: gstvob
 *
 * Created on 03 de Junho de 2019, 15:20
 */

#ifndef WHILE_H
#define WHILE_H

#include "ModelComponent.h"
#include "Plugin.h"
#include "Queue.h"

class While : public ModelComponent {

public:
    While(Model* model);
    While(const While& orig);
    virtual ~While();

    virtual std::string show();
    static PluginInformation* GetPluginInformation();
    static ModelComponent* LoadInstance(Model* model, std::map<std::string, std::string>* fields);

    void set_label(std::string value) {
        _label = value; 
    }
    void set_endwhile_label(std::string value) {
        _endwhile_label = value;
    }

    void set_condition(std::string expr) {
        _condition = expr;
    }

    std::string get_label(std::string value) {
        return _label; 
    }
    std::string get_endwhile_label(std::string value) {
        return _endwhile_label;
    }


protected:
    virtual void _execute(Entity* entity);
    virtual bool _loadInstance(std::map<std::string, std::string>* fields);
    virtual void _initBetweenReplications();
    virtual std::map<std::string, std::string>* _saveInstance();
    virtual bool _check(std::string* errorMessage);

private:
    std::string _condition = "";
    std::string _label = "";
    std::string _endwhile_label = "";
};

#endif /* HOLD_H */
