#pragma once

#include <sol/forward.hpp>

#include <Usagi/Core/Element.hpp>

#include "TransitionableImage.hpp"

namespace usagi
{
struct TransformComponent;
class AssetRoot;
}

namespace usagi::negi
{
class Scene;
struct SpriteComponent;

class ImageLayer : public TransitionableImage
{
    Scene *mScene = nullptr;

public:
    ImageLayer(Element *parent, std::string name, float y_pos, Scene *scene);

    // todo pass in texture by scene
    void changeImage(const std::string &locator);

    static void exportScript(sol::table ns);
};
}
