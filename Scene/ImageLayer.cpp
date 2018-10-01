#include "ImageLayer.hpp"

#include <Usagi/Core/Logging.hpp>
#include <Usagi/Asset/Helper/Load.hpp>
#include <Usagi/Transform/TransformComponent.hpp>
#include <Usagi/Runtime/Graphics/GpuImage.hpp>

#include <MoeLoop/MoeLoopGame.hpp>
#include <MoeLoop/Script/Lua.hpp>

#include "Scene.hpp"

namespace usagi::moeloop
{
ImageLayer::ImageLayer(
    Element *parent,
    std::string name,
    float y_pos,
    Scene *scene)
    : TransitionableImage(parent, std::move(name))
    , mScene(scene)
{
    comp<TransformComponent>()->setPosition({ 0, y_pos, 0 });
}

void ImageLayer::changeImage(const std::string &locator)
{
    LOG(info, "ImageLayer::changeImage {}", locator);
    const auto texture = loadTexture(mScene->game(), locator);
    switchImage(1.0, "linear", texture);
    comp<TransformComponent>()->setOffset({
        0, 0, static_cast<float>(texture->size().y())
    });
}

void ImageLayer::exportScript(kaguya::State &vm)
{
    vm["ImageLayer"].setClass(kaguya::UserdataMetatable<ImageLayer>()
        .addFunction("changeImage", &ImageLayer::changeImage)
    );
}
}
