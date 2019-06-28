#include "Load.hpp"

#include <Usagi/Asset/AssetRoot.hpp>
#include <Usagi/Game/Game.hpp>

#include "JsonPropertySheetAssetConverter.hpp"

namespace usagi::negi
{
nlohmann::json loadJson(Game *game, const std::string &locator)
{
    return game->assets()->uncachedRes<JsonPropertySheetAssetConverter>(
        locator
    );
}
}
