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

#include "Scene/Scene.hpp"
#include "Scene/Character.hpp"
#include "Scene/ImageLayer.hpp"
#include "Game/SceneState.hpp"

namespace usagi::negi
{
NegiGame::NegiGame(std::shared_ptr<Runtime> runtime)
    : GraphicalGame(std::move(runtime))
{
    mInputMapping = mRootElement.addChild<InputMapping>("InputMapping");
    auto mouse = mRuntime->inputManager()->virtualMouse();
    auto kb = mRuntime->inputManager()->virtualKeyboard();
    mouse->addEventListener(mInputMapping);
    kb->addEventListener(mInputMapping);

    {
        auto msg = mInputMapping->addActionGroup("Message");
        // todo allow user configuration
        msg->bindKey("NextMessage", KeyCode::ENTER);
        msg->bindMouseButton("NextMessage", MouseButtonCode::LEFT);
        msg->bindMouseButton("NextMessage", MouseButtonCode::WHEEL_DOWN);
    }
}

NegiGame::~NegiGame()
{
    // remove all states that may reference this game instance.
    mRootElement.removeChild(mStateManager);
}

sol::function NegiGame::loadScript(const std::string &locator)
{
    LOG(info, "Loading script from asset manager: {}", locator);
    return mLua.load(
        assets()->uncachedRes<StringAssetConverter>(locator)
    ).get<sol::function>();
}

void NegiGame::executeFileScript(const std::string &path)
{
    LOG(info, "Executing script from file: {}", path);

    mLua.do_file(std::filesystem::u8path(path).u8string());
}

void NegiGame::executeScript(const std::string &locator)
{
    LOG(info, "Executing script from asset manager: {}", locator);
    mLua.do_string(
        assets()->uncachedRes<StringAssetConverter>(locator)
    );
}

sol::table NegiGame::bindScript()
{
    using namespace std::placeholders;

    // todo
    // mLua.set_exception_handler()

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

    Scene::exportScript(negi);
    Character::exportScript(negi);
    ImageLayer::exportScript(negi);

    return negi;
}

void NegiGame::setupInput()
{
}

void NegiGame::bootstrap()
{
    bindScript();
    // the init script should bootstrap the engine and load the assets
    executeFileScript("bootstrap.lua");
}

void NegiGame::launchGame()
{
    executeFileScript("launch_game.lua");
}

void NegiGame::addFilesystemPackage(
    std::string name,
    const std::string &path)
{
    assets()->addChild<FilesystemAssetPackage>(
        std::move(name), std::filesystem::u8path(path));
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
