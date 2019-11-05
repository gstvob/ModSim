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

#ifndef ENDWHILE_H
#define ENDWHILE_H

#include "ModelComponent.h"
#include "Plugin.h"
#include "Queue.h"

class Endwhile : public ModelComponent {
public:
    Endwhile(Model* model);
    Endwhile(const Endwhile& orig);
    virtual ~Endwhile();

    virtual std::string show();
    static PluginInformation* GetPluginInformation();
    static ModelComponent* LoadInstance(Model* model, std::map<std::string, std::string>* fields);

    void set_label(std::string value) {
        _label = value; 
    }

    std::string get_label(std::string value) {
        return _label; 
    }

    void attach_while(ModelComponent* the_while) {
        this->_attached_while = the_while;
    }

protected:
    virtual void _execute(Entity* entity);
    virtual bool _loadInstance(std::map<std::string, std::string>* fields);
    virtual void _initBetweenReplications();
    virtual std::map<std::string, std::string>* _saveInstance();
    virtual bool _check(std::string* errorMessage);

private:
    std::string _label = "";
    ModelComponent* _attached_while;
};

#endif /* ENDWHILE_H */
