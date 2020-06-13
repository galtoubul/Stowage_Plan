#include "../Interfaces/AlgorithmRegistration.h"
#include "Registrar.h"

AlgorithmRegistration::AlgorithmRegistration(std::function<std::unique_ptr<AbstractAlgorithm>()> algorithmFactory) {
    Registrar::getRegistrar().registerAlgorithmFactory(algorithmFactory);
}
