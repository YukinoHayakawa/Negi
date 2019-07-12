#pragma once

#include <Negi/Script/LuaForward.hpp>
#include <sol/state.hpp>

#include <Usagi/Graphics/Game/GraphicalGame.hpp>

#include "Constants.hpp"

namespace usagi
{
class InputMapping;
class GameState;
}

namespace usagi::negi
{
class Scene;
class SceneState;

class NegiGame : public GraphicalGame
{
protected:
    sol::state mLua;

    InputMapping *mInputMapping = nullptr;

    // return the namespace for our game
    virtual sol::table bindScript();
    void setupInput();

public:
    explicit NegiGame(std::shared_ptr<Runtime> runtime);
    ~NegiGame();

    // execute bootstrap script
    virtual void bootstrap();
    virtual void launchGame();

    InputMapping * inputMapping() { return mInputMapping; }
    sol::state & luaContext() { return mLua; }

    void executeFileScript(const std::string &path);
    sol::function loadAssetScript(
        std::string_view locator,
        std::string_view path_prefix = asset_path_prefix::SCRIPTS);
    void executeAssetScript(
        std::string_view locator,
        std::string_view path_prefix = asset_path_prefix::SCRIPTS);

    // Functions accessible from Lua

    // static void unimplemented(const std::string &msg);
    void addFilesystemPackage(std::string name, const std::string &path);

    void changeState(GameState *state);
    void pushState(GameState *state);
    void popState();

    virtual SceneState * createSceneState(const std::string &name) = 0;
    Scene *currentScene() const;
};
}
