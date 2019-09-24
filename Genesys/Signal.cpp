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

//TODO Colocar o instance of antes daquele cast ali.

void Signal::_execute(Entity* entity) {
    for(int i = 0; i < _holds_waiting_signal->size(); i++) {
        Waiting* waiting = _holds_waiting_signal->getAtRank(i);

        auto component = waiting->getComponent();
        Hold* h = ((Hold*)(component));
        h->release_signal(limit);
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


void Signal::setQueueName(std::string _name) throw() {
    Queue* queue = dynamic_cast<Queue*>(_model->getElementManager()->getElement(Util::TypeOf<Queue>(), _name));
    if (queue != nullptr) {
        _holds_waiting_signal = queue;
    } else {
        throw std::invalid_argument("Queue does not exist");
    }
}

std::string Signal::getQueueName() const {
    return _holds_waiting_signal->getName();
}

ModelComponent* Signal::LoadInstance(Model* model, std::map<std::string, std::string>* fields) {
    Signal* newComponent = new Signal(model);
    try {
    newComponent->_loadInstance(fields);
    } catch (const std::exception& e) {
    }
    return newComponent;
}