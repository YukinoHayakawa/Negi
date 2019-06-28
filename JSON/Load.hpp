#pragma once

#include "JSON.hpp"

namespace usagi
{
class Game;
}

namespace usagi::negi
{
nlohmann::json loadJson(Game *game, const std::string &locator);
}
