#pragma once

#include "Decomposer.h"
#include <functional>
#include <map>
#include <memory>
#include <string>

namespace df {
namespace attributes {

/**
 * @brief Factory for creating Decomposer instances given a name
 * @ingroup Attributes
 */
class DecomposerFactory {
  public:
    // using Creator = std::function<std::unique_ptr<Decomposer>()>;

    static void bind(const std::string &name, std::shared_ptr<Decomposer>);
    static std::unique_ptr<Decomposer> create(const std::string &name);
    static bool isRegistered(const std::string &name);
    static std::vector<std::string> names();

  private:
    static DecomposerFactory &instance();
    std::unique_ptr<Decomposer> clone(const Decomposer *prototype) const;
    DecomposerFactory() = default;
    ~DecomposerFactory() = default;

    DecomposerFactory(const DecomposerFactory &) = delete;
    DecomposerFactory &operator=(const DecomposerFactory &) = delete;

    std::map<std::string, std::shared_ptr<Decomposer>> prototypes_;
};

void registerAllDecomposer();

} // namespace attributes
} // namespace df