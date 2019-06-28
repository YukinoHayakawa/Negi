#include "NegiGame.hpp"

#include <fstream>

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

kaguya::LuaFunction NegiGame::loadScript(const std::string &locator)
{
    LOG(info, "Loading script from asset manager: {}", locator);
    return mLuaContext.loadstring(
        assets()->uncachedRes<StringAssetConverter>(locator)
    );
}

void NegiGame::executeFileScript(const std::string &path)
{
    LOG(info, "Executing script from file: {}", path);
    std::ifstream in(std::filesystem::u8path(path));
    mLuaContext.dostream(in);
}

void NegiGame::executeScript(const std::string &locator)
{
    LOG(info, "Executing script from asset manager: {}", locator);
    mLuaContext.dostring(
        assets()->uncachedRes<StringAssetConverter>(locator)
    );
}

void NegiGame::unimplemented(const std::string &msg)
{
    LOG(warn, "Script: unimplemented code: {}", msg);
}

void NegiGame::bindScript()
{
    using namespace std::placeholders;

    mLuaContext.setErrorHandler([](const int error, const char *msg) {
        LOG(error, "Lua error {}: {}", error, msg);
        throw std::runtime_error("Error occurred in script.");
    });

    mLuaContext["unimplemented"].setFunction(&NegiGame::unimplemented);
    mLuaContext["MoeLoop"].setClass(kaguya::UserdataMetatable<NegiGame>()
        .addFunction("addFilesystemPackage", &NegiGame::addFilesystemPackage)
        .addFunction("changeState", &NegiGame::changeState)
        .addFunction("pushState", &NegiGame::pushState)
        .addFunction("popState", &NegiGame::popState)
        .addFunction("createSceneState", &NegiGame::createSceneState)
        .addFunction("currentScene", &NegiGame::currentScene)
    );

    Scene::exportScript(mLuaContext);
    Character::exportScript(mLuaContext);
    ImageLayer::exportScript(mLuaContext);
}

void NegiGame::setupInput()
{
}

void NegiGame::init()
{
    // the init script should bootstrap the engine, load the assets, then
    // switch to other states that is supposed to interact with the player.
    executeFileScript("init.lua");
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
