#ifndef SIGNAL_H
#define SIGNAL_H
#include "ModelComponent.h"

class Signal: public ModelComponent {
public:
    Signal(Model *model);
    Signal(const Signal& orig);
    virtual ~Signal();
    virtual std::string show();
    static PluginInformation* GetPluginInformation();
    static ModelComponent* LoadInstance(Model* model, std::map<std::string, std::string>* fields);
    void setSignalName(std::string name) {
        signalName = name;
    }
    std::string getSignalName() {
        return signalName;
    }
private:
    int limit = 10;
    std::string signalName;

protected:
    virtual void _execute(Entity* entity);
    virtual void _initBetweenReplications();

};

#endif