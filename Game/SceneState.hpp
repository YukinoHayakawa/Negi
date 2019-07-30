#pragma once

#include <Negi/Script/LuaForward.hpp>
#include <sol/thread.hpp>
#include <sol/coroutine.hpp>

#include <Usagi/Camera/Controller/CameraMan.hpp>

#include "NegiGameState.hpp"

namespace usagi
{
class AnimationSystem;
class StaticCameraController;
class OrthogonalCamera;
}

namespace usagi::negi
{
class SortedSpriteRenderingSystem;
class Scene;

class SceneState : public NegiGameState
{
protected:
    Scene *mScene = nullptr;
    sol::thread mSceneThread;
    sol::coroutine mSceneScript;
    bool mContinueScript = true;

    using ModelCameraMan =
        CameraMan<OrthogonalCamera, StaticCameraController>;
    ModelCameraMan *mCameraElement = nullptr;

    AnimationSystem *mAnimation = nullptr;
    SortedSpriteRenderingSystem *mSpriteSystem = nullptr;

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
