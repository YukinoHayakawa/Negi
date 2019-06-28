#pragma once

#include "JSONForwardDecl.hpp"

namespace usagi::negi
{
struct JsonAssetDecoder
{
    json operator()(std::istream &in) const;
};
}
