#include "Registrar.h"
#include <dlfcn.h>
#include <iostream>

Registrar Registrar::registrar;

void Registrar::DlCloser::operator()(void *dlhandle) const noexcept{
    dlclose(dlhandle);
}

Registrar& Registrar::getRegistrar(){
    return registrar;
}

Registrar::~Registrar(){
    // Remove all factories - before closing all handles!
    factoryVec.clear();
    algNamesVec.clear();
    algorithmMap.clear();
    notRegisteredAlgVec.clear();
    handles.clear();
}

bool Registrar::loadAlgorithmFromFile(const char* filePath, std::string& error, const std::string& algName)
{
    // Try to load given module
    std::unique_ptr<void, DlCloser> algoHandle(dlopen(filePath, RTLD_LAZY));

    // Check if dlopen succeeded
    if (algoHandle != nullptr) {
        handles[filePath] = std::move(algoHandle);

        if(algNamesVec.size() < factoryVec.size()){
            algNamesVec.emplace_back(algName);
            int pos = std::distance(algNamesVec.begin(), std::find(algNamesVec.begin(), algNamesVec.end(), algName));
            std::function<std::unique_ptr<AbstractAlgorithm>()>& algorithmFactory = getAlgorithmFactoryVector().at(pos);
            algorithmMap.insert(std::make_pair(algName, algorithmFactory));
        } else
            notRegisteredAlgVec.emplace_back(algName);
        return true;
    } else {
        const char *dlopen_error = dlerror();
        error = dlopen_error ? dlopen_error : "Failed to open shared object!";
        return false;
    }
}
