#pragma once

#include <Negi/Script/LuaForward.hpp>
#include <sol/state.hpp>

#include <Usagi/Graphics/Game/GraphicalGame.hpp>

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

    static void luaPanic(std::optional<std::string> msg);

public:
    explicit NegiGame(std::shared_ptr<Runtime> runtime);
    ~NegiGame();

    // execute bootstrap script
    virtual void bootstrap();
    virtual void launchGame();

    InputMapping * inputMapping() { return mInputMapping; }
    sol::state & luaContext() { return mLua; }

    sol::function loadScript(const std::string &locator);
    void executeFileScript(const std::string &path);
    void executeScript(const std::string &locator);

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
