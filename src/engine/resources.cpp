#include "resources.hpp"

namespace engine {
Filesystem::Filesystem() noexcept
{
    root_ = std::filesystem::current_path() / "content";
}

string Filesystem::resolve(string relative) noexcept
{
    std::filesystem::path absilut = root_ / relative;
    return absilut.string();
}

} // namespace engine
