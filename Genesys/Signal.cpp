#include "Signal.h"
#include "Entity.h"
#include "Hold.h"
#include "Model.h"
#include "Util.h"

Signal::Signal(Model *model): ModelComponent(model, Util::TypeOf<Signal>()) {

}

Signal::Signal(const Signal& orig): ModelComponent(orig) {

}


Signal::~Signal() {
}
template<typename Base, typename T>
inline bool instanceof(const T*) {
   return std::is_base_of<Base, T>::value;
}

void Signal::_execute(Entity* entity) {
    std::list<ModelComponent*>::iterator it = _model->getComponentManager()->begin();
    for (; it != _model->getComponentManager()->end(); it++) {
        auto component = *it;
        if (instanceof<Hold>(*it)) {
            Hold* h = ((Hold*)(*it));
            if (h->getWaitForValueExpr() == signalName) {
                h->release_signal(limit);
            }
        }
    }
}
std::string Signal::show() {
    return ModelComponent::show() + "";
}
PluginInformation* Signal::GetPluginInformation() {
    return new PluginInformation(Util::TypeOf<Signal>(), &Hold::LoadInstance);
}

void Signal::_initBetweenReplications() {
    
}

ModelComponent* Signal::LoadInstance(Model* model, std::map<std::string, std::string>* fields) {
    Signal* newComponent = new Signal(model);
    try {
    newComponent->_loadInstance(fields);
    } catch (const std::exception& e) {
    }
    return newComponent;
}