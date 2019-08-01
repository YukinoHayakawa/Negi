#include "ImageLayer.hpp"

#include <sol/table.hpp>

#include <Usagi/Core/Logging.hpp>
#include <Usagi/Asset/Helper/Load.hpp>
#include <Usagi/Transform/TransformComponent.hpp>
#include <Usagi/Runtime/Graphics/GpuImage.hpp>

#include <Negi/Constants.hpp>
#include <Negi/NegiGame.hpp>

#include "Scene.hpp"

namespace usagi::negi
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
    using namespace asset_path_prefix;
    LOG(info, "ImageLayer::changeImage {}", locator);
    const auto texture = loadTexture(mScene->game(), IMAGES + locator);
    switchImage(0.5, "inOutSine", texture);
    comp<TransformComponent>()->setOffset({
        0, 0, static_cast<float>(texture->size().y())
    });
}

void ImageLayer::exportScript(sol::table ns)
{
    ns.new_usertype<ImageLayer>(
        "ImageLayer",
        "changeImage", &ImageLayer::changeImage
    );
}
}
