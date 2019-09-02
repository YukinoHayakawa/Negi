#include "SceneState.hpp"

#include <Usagi/Animation/AnimationSystem.hpp>
#include <Usagi/Asset/AssetRoot.hpp>
#include <Usagi/Camera/Controller/StaticCameraController.hpp>
#include <Usagi/Camera/OrthogonalCamera.hpp>
#include <Usagi/Core/Logging.hpp>
#include <Usagi/Graphics/Game/GraphicalGame.hpp>
#include <Usagi/Runtime/Graphics/Swapchain.hpp>
#include <Usagi/Transform/TransformComponent.hpp>
#include <Usagi/Utility/Functional.hpp>
#include <Usagi/Interactive/InputMapping.hpp>

#include <Negi/JSON/JsonPropertySheetAssetConverter.hpp>
#include <Negi/NegiGame.hpp>
#include <Negi/Render/SortedSpriteRenderingSystem.hpp>
#include <Negi/Render/SpriteComponent.hpp>
#include <Negi/Scene/Scene.hpp>
#include <Negi/Constants.hpp>

namespace usagi::negi
{
void SceneState::loadScene()
{
    using namespace asset_path_prefix;
    LOG(info, "Loading scene: {}", name());

    mScene = addChild<Scene>(name(), game());
    mScene->load(game()->assets()->uncachedRes<JsonPropertySheetAssetConverter>(
        fmt::format("{}{}.json", SCENES, name())
    ));

    mSceneThread = sol::thread::create(game()->luaContext());
    mSceneScript = {
        mSceneThread.state(),
        game()->loadAssetScript(name() + ".lua", SCENE_SCRIPTS)
    };
}

void SceneState::createCamera()
{
    mCameraElement = addChild<ModelCameraMan>(
        "CameraMan",
        std::make_shared<OrthogonalCamera>(),
        std::make_shared<StaticCameraController>());
}

void SceneState::setupRenderer()
{
    mSpriteSystem = addSystem(
        "sprite",
        std::make_unique<SortedSpriteRenderingSystem>(
            game(), [](TransformComponent *lt, SpriteComponent *ls,
            TransformComponent *rt, SpriteComponent *rs) {
                return lt->position().y() > rt->position().y();
            }
        ));
    mSpriteSystem->setWorldToNdcFunc([=]() {
        // world -> camera local -> NDC
        return mCameraElement->camera()->localToNDC() *
            mCameraElement->comp<TransformComponent>()->worldToLocal();
    });
}

void SceneState::setupAnimation()
{
    mAnimation = addSystem(
        "animation",
        std::make_unique<AnimationSystem>()
    );
}

void SceneState::setupInput()
{
    const auto im = game()->inputMapping();
    {
        auto msg = im->actionGroup("Message");
        msg->activate();
        // bind with virtual function
        msg->setBinaryActionHandler("NextMessage", partial_apply(
            &SceneState::playerContinueScript, this));
    }
}

SceneState::SceneState(Element *parent, std::string name, NegiGame *game)
    : NegiGameState(parent, name, game)
{
    createCamera();
    // animation system must be inserted before rendering, otherwise outdated
    // content from last frame will be rendered
    setupAnimation();
    setupRenderer();
    loadScene();
}

void SceneState::continueScript()
{
    if(mSceneScript.runnable())
    {
        const auto ret = mSceneScript();
        if(!ret.valid())
        {
            sol::script_default_on_error(ret.lua_state(), std::move(ret));
        }
    }
}

bool SceneState::continueScriptCondition()
{
    return mContinueScript;
}

void SceneState::update(const Clock &clock)
{
    const Vector2f size = mGame->mainWindow()->swapchain->size().cast<float>();
    mCameraElement->camera()->setOrthogonal(
        0, size.x(), 0, size.y(), -100, 100);

    // todo preload resources
    if(continueScriptCondition())
    {
        continueScript();
        mContinueScript = false;
    }

    NegiGameState::update(clock);
}

void SceneState::pause()
{
}

void SceneState::resume()
{
    setupInput();
}

void SceneState::finishCurrentDialogOrContinue()
{
    // newly pushed animation not counted...
    if(mAnimation->activeCount() > 0)
    {
        mAnimation->immediatelyFinishAll();
    }
    else
    {
        mContinueScript = true;
    }
}

void SceneState::playerContinueScript(const bool yes)
{
    if(yes)
    {
        finishCurrentDialogOrContinue();
    }
}
}
