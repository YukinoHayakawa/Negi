#pragma once

#include "JSON.hpp"

namespace usagi
{
class Game;
}

namespace usagi::moeloop
{
nlohmann::json loadJson(Game *game, const std::string &locator);
}
