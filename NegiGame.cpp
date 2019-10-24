#include "NegiGame.hpp"

#include <Usagi/Asset/AssetRoot.hpp>
#include <Usagi/Asset/Package/Filesystem/FilesystemAssetPackage.hpp>
#include <Usagi/Asset/Converter/Uncached/StringAssetConverter.hpp>
#include <Usagi/Core/Logging.hpp>
#include <Usagi/Runtime/Input/InputManager.hpp>
#include <Usagi/Runtime/Input/Mouse/Mouse.hpp>
#include <Usagi/Runtime/Input/Keyboard/Keyboard.hpp>
#include <Usagi/Runtime/Runtime.hpp>
#include <Usagi/Game/GameStateManager.hpp>
#include <Usagi/Interactive/InputMapping.hpp>
#include <Usagi/Utility/Unicode.hpp>

#include "Scene/Scene.hpp"
#include "Scene/Character.hpp"
#include "Scene/ImageLayer.hpp"
#include "Game/SceneState.hpp"
#include "Scene/AudioTrack.hpp"

namespace usagi::negi
{
NegiGame::NegiGame(std::shared_ptr<Runtime> runtime)
    : GraphicalGame(std::move(runtime))
{
    mRuntime->initAudio();

    mLua.open_libraries(
        sol::lib::base,
        sol::lib::coroutine
    );

    mInputMapping = mRootElement.addChild<InputMapping>("InputMapping");
    auto mouse = mRuntime->inputManager()->virtualMouse();
    auto kb = mRuntime->inputManager()->virtualKeyboard();
    mouse->addEventListener(mInputMapping);
    kb->addEventListener(mInputMapping);

    {
        auto msg = mInputMapping->addActionGroup("Message");
        // todo allow user configuration
        msg->bindKey("NextMessage", KeyCode::ENTER);
        msg->bindKey("NextMessage", KeyCode::SPACE);
        msg->bindMouseButton("NextMessage", MouseButtonCode::LEFT);
        msg->bindMouseButton("NextMessage", MouseButtonCode::WHEEL_DOWN);
    }
}

NegiGame::~NegiGame()
{
    // remove all states that may reference this game instance.
    mRootElement.removeChild(mStateManager);
}

void NegiGame::executeFileScript(const std::string &path)
{
    LOG(info, "Executing script from file: {}", path);

    mLua.safe_script_file(path);
}

sol::function NegiGame::loadAssetScript(
    std::string_view locator,
    std::string_view path_prefix)
{
    auto path = fmt::format("{}{}", path_prefix, locator);
    LOG(info, "Loading script from asset manager: {}", path);
    return mLua.load(
        assets()->uncachedRes<StringAssetConverter>(path)
    ).get<sol::function>();
}

void NegiGame::executeAssetScript(
    std::string_view locator,
    std::string_view path_prefix)
{
    auto path = fmt::format("{}{}", path_prefix, locator);
    LOG(info, "Executing script from asset manager: {}", path);
    mLua.safe_script(
        assets()->uncachedRes<StringAssetConverter>(path)
    );
}

sol::table NegiGame::bindScript()
{
    // todo remove. replace with warning
    // // mLuaContext["unimplemented"].setFunction(&NegiGame::unimplemented);

    // namespace negi in Lua
    sol::table negi = mLua["negi"].get_or_create<sol::table>();
    negi.new_usertype<NegiGame>(
        "NegiGame",
        "addFilesystemPackage", &NegiGame::addFilesystemPackage,
        "changeState", &NegiGame::changeState,
        "pushState", &NegiGame::pushState,
        "popState", &NegiGame::popState,
        "createSceneState", &NegiGame::createSceneState,
        "currentScene", &NegiGame::currentScene
    );

    // register SceneStage conversion
    negi.new_usertype<GameState>(
        "GameState"
    );
    negi.new_usertype<SceneState>(
        "SceneState",
        sol::base_classes, sol::bases<GameState>()
    );

    Scene::exportScript(negi);
    Character::exportScript(negi);
    ImageLayer::exportScript(negi);
    AudioTrack::exportScript(negi);

    return negi;
}

void NegiGame::setupInput()
{
}

void NegiGame::bootstrap()
{
    bindScript();
    // the init script should bootstrap the engine and load the assets
    executeAssetScript("bootstrap.lua", "");
}

void NegiGame::launchGame()
{
    executeAssetScript("launch_game.lua", "");
}

void NegiGame::addFilesystemPackage(
    std::string name,
    const std::string &path)
{
    assets()->addChild<FilesystemAssetPackage>(
        std::move(name), stringToU8string(path));
}

void NegiGame::changeState(GameState *state)
{
    mInputMapping->disableAllActionGroups();
    mStateManager->changeState(state);
}

void NegiGame::pushState(GameState *state)
{
    mInputMapping->disableAllActionGroups();
    mStateManager->pushState(state);
}

void NegiGame::popState()
{
    mStateManager->popState();
}

Scene * NegiGame::currentScene() const
{
    if(const auto state = dynamic_cast<SceneState*>(mStateManager->topState()))
    {
        return state->scene();
    }
    return nullptr;
}
}
