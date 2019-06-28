#pragma once

#include <Usagi/Graphics/Game/GraphicalGame.hpp>

#include "Script/Lua.hpp"

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
    kaguya::State mLuaContext;

    InputMapping *mInputMapping = nullptr;

    void bindScript();
    void setupInput();

public:
    explicit NegiGame(std::shared_ptr<Runtime> runtime);
    ~NegiGame();

    /**
     * \brief Push InitGameState and execute init script.
     */
    virtual void init();

    InputMapping * inputMapping() { return mInputMapping; }
    kaguya::State * luaContext() { return &mLuaContext; }

    kaguya::LuaFunction loadScript(const std::string &locator);
    void executeFileScript(const std::string &path);
    void executeScript(const std::string &locator);

    // Functions accessible from Lua

    static void unimplemented(const std::string &msg);
    void addFilesystemPackage(std::string name, const std::string &path);

    void changeState(GameState *state);
    void pushState(GameState *state);
    void popState();

    virtual SceneState * createSceneState(const std::string &name) = 0;
    Scene *currentScene() const;
};
}
