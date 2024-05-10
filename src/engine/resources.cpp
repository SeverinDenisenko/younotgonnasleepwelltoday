#include "resources.hpp"

namespace engine {
Filesystem::Filesystem(string root) noexcept
{
    root_ = std::move(root);
}

string Filesystem::resolve(string relative) noexcept
{
    std::filesystem::path absilut = root_ / relative;
    return absilut.string();
}

} // namespace engine
