#include "Scene.hpp"

#include <sol/table.hpp>

#include <Usagi/Asset/AssetRoot.hpp>
#include <Usagi/Asset/Helper/Load.hpp>
#include <Usagi/Core/Logging.hpp>

#include <Negi/JSON/Load.hpp>
#include <Negi/JSON/Math.hpp>
#include <Negi/NegiGame.hpp>

#include "Expression.hpp"
#include "ImageLayer.hpp"
#include "Character.hpp"
#include "CharacterMessageEvent.hpp"

namespace usagi::negi
{
Scene::Scene(Element *parent, std::string name, NegiGame *game)
    : Element(parent, std::move(name))
    , mGame(game)
{
    mCharacters = addChild("Characters");
    mExpressions = addChild("Expressions");
    mImageLayers = addChild("ImageLayers");

    addEventListener<CharacterMessageEvent>([&](CharacterMessageEvent &e) {
        mLastSpeakCharacter = e.character;
    });
}

void Scene::load(const json &j)
{
    // todo exchange z and y to correct order
    auto size = j["size"].get<Vector3f>();
    mBound.min() = { 0, -size.z(), 0 };
    mBound.max() = { size.x(), +size.z(), size.y() };

    mPositions = j["positions"].get<decltype(mPositions)>();

    for(auto &&i : j["imageLayers"].get<std::map<std::string, float>>())
    {
        mImageLayers->addChild<ImageLayer>(i.first, i.second, this);
    }
}

Character * Scene::loadCharacter(const std::string &asset_locator)
{
    return mCharacters->addChild<Character>(asset_locator);
}

Expression * Scene::loadExpression(const std::string &name)
{
    LOG(info, "Loading expression: {}", name);
    const auto data = loadJson(mGame, fmt::format("expressions/{}.json", name));
    return mExpressions->addChild<Expression>(
        name,
        data["origin"].get<Vector2f>(),
        data["face_center"].get<Vector2f>(),
        loadTexture(mGame, "expressions/" + data["image"].get<std::string>())
    );
}

Vector3f Scene::getPosition(const std::string &name) const
{
    const auto iter = mPositions.find(name);
    if(iter == mPositions.end())
    {
        LOG(error, "No such position: {}", name);
        throw std::runtime_error("Non-existing position");
    }
    return iter->second;
}

ImageLayer * Scene::getImageLayer(const std::string &name)
{
    return mImageLayers->findChild(name)->as<ImageLayer>();
}

void Scene::playSoundEffect(std::string_view name)
{
    // todo
}

void Scene::playMusic(std::string_view name)
{
    // todo
}

void Scene::exportScript(sol::table ns)
{
    ns.new_usertype<Scene>(
        "Scene",
        "loadCharacter", &Scene::loadCharacter,
        "loadExpression", &Scene::loadExpression,
        "getImageLayer", &Scene::getImageLayer,
        "getPosition", &Scene::getPosition,
        "playSoundEffect", &Scene::playSoundEffect,
        "playMusic", &Scene::playMusic
    );
}
}
