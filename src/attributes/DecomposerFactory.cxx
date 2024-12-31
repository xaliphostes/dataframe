#include <dataframe/attributes/DecomposerFactory.h>

#include <dataframe/attributes/Area.h>
#include <dataframe/attributes/Components.h>
#include <dataframe/attributes/Coordinates.h>
#include <dataframe/attributes/EigenValues.h>
#include <dataframe/attributes/EigenVectors.h>
#include <dataframe/attributes/Normals.h>
#include <dataframe/attributes/Valence.h>

namespace df {
namespace attributes {

// ================================================================================

void registerAllDecomposer() {
    if (DecomposerFactory::isRegistered("Coordinates")) {
        return;
    }

    DecomposerFactory::bind("Coordinates", std::make_shared<Coordinates>());
    DecomposerFactory::bind("Area", std::make_shared<Area>());
    DecomposerFactory::bind("Components", std::make_shared<Components>());
    DecomposerFactory::bind("EigenValues", std::make_shared<EigenValues>());
    DecomposerFactory::bind("EigenVectors", std::make_shared<EigenVectors>());
    DecomposerFactory::bind("Normals", std::make_shared<Normals>());
    DecomposerFactory::bind("Valence", std::make_shared<Valence>());
}

// ================================================================================

/**
 * @brief Get the singleton instance of the factory
 */
DecomposerFactory &DecomposerFactory::instance() {
    static DecomposerFactory factory;
    return factory;
}

/**
 * @brief Create a new instance by name
 * @param name The name of the registered decomposer type
 * @return std::shared_ptr<Decomposer> A new cloned instance
 */
std::unique_ptr<Decomposer> DecomposerFactory::create(const std::string &name) {
    auto it = DecomposerFactory::instance().prototypes_.find(name);
    if (it == DecomposerFactory::instance().prototypes_.end()) {
        throw std::runtime_error("Unknown decomposer type: " + name);
    }
    return it->second->clone();
}

/**
 * @brief Create a new instance by cloning an existing decomposer
 * @param prototype The decomposer to clone
 * @return std::shared_ptr<Decomposer> A new cloned instance
 */
std::unique_ptr<Decomposer>
DecomposerFactory::clone(const Decomposer *prototype) const {
    if (!prototype) {
        throw std::runtime_error("Cannot clone null prototype");
    }
    return prototype->clone();
}

/**
 * @brief Check if a decomposer type is registered
 */
bool DecomposerFactory::isRegistered(const std::string &name) {
    return instance().prototypes_.find(name) != instance().prototypes_.end();
}

/**
 * @brief Get all registered decomposer names
 */
std::vector<std::string> DecomposerFactory::names() {
    std::vector<std::string> names;
    names.reserve(instance().prototypes_.size());
    for (const auto &pair : instance().prototypes_) {
        names.push_back(pair.first);
    }
    return names;
}

/**
 * @brief Register a new decomposer type with its creator function
 * @param name The name of the decomposer class
 * @param creator Function that creates a new instance of the decomposer
 * @code
 * using namespace df::attributes;
 *
 * DecomposerFactory::bind<CoordinatesDecomposer>("Coordinates");
 * @endcode
 */
inline void DecomposerFactory::bind(const std::string &name,
                                    std::shared_ptr<Decomposer> decomposer) {
    instance().prototypes_[name] = decomposer;
}

} // namespace attributes
} // namespace df
