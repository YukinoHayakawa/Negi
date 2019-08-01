#include "CharacterAvatar.hpp"

#include <Negi/Render/SpriteComponent.hpp>
#include <Usagi/Transform/TransformComponent.hpp>

#include "Expression.hpp"

namespace usagi::negi
{
CharacterAvatar::CharacterAvatar(Element *parent, std::string name)
    : TransitionableImage(parent, std::move(name))
{
}

void CharacterAvatar::hide()
{
    switchImage(0.5, "inOutSine", { });
}

void CharacterAvatar::change(Expression *expr)
{
    switchImage(0.5, "inOutSine", expr->texture());
    const auto o = expr->faceCenter();
    comp<SpriteComponent>()->layers[1].offset = -o;
}

void CharacterAvatar::setPosition(const Vector3f &position)
{
    comp<TransformComponent>()->setPosition(position);
}
}
