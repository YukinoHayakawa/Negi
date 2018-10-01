﻿#include "Scene.hpp"

#include <Usagi/Asset/AssetRoot.hpp>
#include <Usagi/Asset/Helper/Load.hpp>
#include <Usagi/Core/Logging.hpp>

#include <MoeLoop/Script/Lua.hpp>
#include <MoeLoop/JSON/Load.hpp>
#include <MoeLoop/JSON/Math.hpp>
#include <MoeLoop/MoeLoopGame.hpp>

#include "Expression.hpp"
#include "ImageLayer.hpp"
#include "Character.hpp"

namespace usagi::moeloop
{
Scene::Scene(Element *parent, std::string name, MoeLoopGame *game)
    : Element(parent, std::move(name))
    , mGame(game)
{
    mCharacters = addChild("Characters");
    mExpressions = addChild("Expressions");
    mImageLayers = addChild("ImageLayers");
}

void Scene::load(const json &j)
{
    auto size = j["size"].get<Vector3f>();
    mBound.min() = { 0, -size.z(), 0 };
    mBound.max() = { size.x(), +size.z(), size.y() };

    mPositions = j["positions"].get<decltype(mPositions)>();
}

ImageLayer * Scene::createImageLayer(const std::string &name, float y_pos)
{
    return mImageLayers->addChild<ImageLayer>(name, y_pos, this);
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

void Scene::exportScript(kaguya::State &vm)
{
    vm["Scene"].setClass(kaguya::UserdataMetatable<Scene>()
        .addFunction("createImageLayer", &Scene::createImageLayer)
        .addFunction("loadCharacter", &Scene::loadCharacter)
        .addFunction("loadExpression", &Scene::loadExpression)
        .addFunction("getPosition", &Scene::getPosition)
    );
}
}
