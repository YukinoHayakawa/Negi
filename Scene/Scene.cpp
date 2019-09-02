#include "Scene.hpp"

#include <sol/table.hpp>

#include <Usagi/Asset/AssetRoot.hpp>
#include <Usagi/Asset/Helper/Load.hpp>
#include <Usagi/Core/Logging.hpp>

#include <Negi/JSON/Load.hpp>
#include <Negi/JSON/Math.hpp>
#include <Negi/NegiGame.hpp>
#include <Negi/Constants.hpp>

#include "Expression.hpp"
#include "ImageLayer.hpp"
#include "AudioTrack.hpp"
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
    mAudioTracks = addChild("AudioTracks");

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

    for(auto &&i : j["image_layers"].get<std::map<std::string, float>>())
    {
        mImageLayers->addChild<ImageLayer>(i.first, i.second, this);
    }

    for(auto &&i : j["audio_tracks"].get<std::map<std::string, json>>())
    {
        auto &&obj = i.second;
        // todo set track params
        mAudioTracks->addChild<AudioTrack>(
            i.first,
            // i.second,
            this
        );
    }
}

Character * Scene::loadCharacter(const std::string &name)
{
    using namespace asset_path_prefix;
    auto c =  mCharacters->addChild<Character>(name);
    const auto data = loadJson(mGame, fmt::format(
        "{}{}.json", CHARACTERS, name));
    c->load(data);
    return c;
}

Expression * Scene::loadExpression(const std::string &name)
{
    using namespace asset_path_prefix;
    LOG(info, "Loading expression: {}", name);
    const auto data = loadJson(mGame, fmt::format(
        "{}{}.json", EXPRESSIONS, name));
    return mExpressions->addChild<Expression>(
        name,
        data["origin"].get<Vector2f>(),
        data["face_center"].get<Vector2f>(),
        loadTexture(mGame, EXPRESSIONS + data["image"].get<std::string>())
    );
}

Vector3f Scene::getPosition(const std::string &name) const
{
    const auto iter = mPositions.find(name);
    if(iter == mPositions.end())
    {
        LOG(error, "No such position: {}", name);
        USAGI_THROW(std::runtime_error("Non-existing position"));
    }
    return iter->second;
}

ImageLayer * Scene::getImageLayer(const std::string &name)
{
    return mImageLayers->findChild(name)->as<ImageLayer>();
}

AudioTrack *Scene::getAudioTrack(const std::string &name)
{
    return mAudioTracks->findChild(name)->as<AudioTrack>();
}

void Scene::exportScript(sol::table ns)
{
    ns.new_usertype<Scene>(
        "Scene",
        "loadCharacter", &Scene::loadCharacter,
        "loadExpression", &Scene::loadExpression,
        "getImageLayer", &Scene::getImageLayer,
        "getPosition", &Scene::getPosition,
        "getAudioTrack", &Scene::getAudioTrack
    );
}
}
