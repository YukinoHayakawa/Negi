#pragma once

#include <Usagi/Camera/Controller/CameraMan.hpp>

#include <Negi/Script/Lua.hpp>

#include "NegiGameState.hpp"

namespace usagi
{
class AnimationSubsystem;
class StaticCameraController;
class OrthogonalCamera;
}

namespace usagi::negi
{
class Scene;

class SceneState : public NegiGameState
{
protected:
    Scene *mScene = nullptr;
    kaguya::LuaThread mSceneThread;
    kaguya::LuaFunction mSceneScript;
    bool mContinueScript = true;

    using ModelCameraMan =
        CameraMan<OrthogonalCamera, StaticCameraController>;
    ModelCameraMan *mCameraElement = nullptr;

    AnimationSubsystem *mAnimation = nullptr;

    void loadScene();
    void createCamera();
    void setupRenderer();
    void setupAnimation();
    void setupInput();

    void continueScript();

public:
    SceneState(Element *parent, std::string name, NegiGame *game);

    Scene * scene() const { return mScene; }

    void update(const Clock &clock) override;
    void pause() override;
    void resume() override;

    void playerContinueScript(bool yes);
};
}
