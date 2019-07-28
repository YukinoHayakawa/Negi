#include "Character.hpp"

#include <sol/table.hpp>

#include <Usagi/Transform/TransformComponent.hpp>
#include <Usagi/Animation/AnimationComponent.hpp>
#include <Usagi/Core/Logging.hpp>
#include <Usagi/Math/Lerp.hpp>

#include <Negi/Render/SpriteComponent.hpp>

#include "Expression.hpp"
#include "CharacterMessageEvent.hpp"

namespace usagi::negi
{
Character::Character(Element *parent, std::string name)
    : TransitionableImage(parent, std::move(name))
{
}

void Character::setPosition(const Vector3f &position)
{
    comp<TransformComponent>()->setPosition(position);
}

void Character::changeExpression(Expression *expr)
{
    // todo use scene default

    switchImage(0.5, "linear", expr->texture());
    const auto o = expr->origin();
    comp<SpriteComponent>()->layers[1].offset = -o;
}

void Character::showName(const bool show)
{
    mShowName = show;
}

void Character::showAvatar(const bool show)
{
    mShowAvatar = show;
}

void Character::changePosition(const Vector3f &position)
{
    Animation ani;
    ani.duration = 0.5;
    ani.animation_func = [
        this,
        start_pos = comp<TransformComponent>()->position(),
        position
    ](const double t) {
        setPosition(lerp(t, start_pos, position));
    };
    comp<AnimationComponent>()->add(std::move(ani));
}

void Character::say(const std::string &text)
{
    LOG(info, "{}: {}", name(), text);
    mLastName = name();
    mLastMessage = text;
    sendEvent<CharacterMessageEvent>(this);
}

void Character::setAlias(std::string_view alias)
{

}

void Character::removeAlias()
{
}

void Character::enterStage(
    Expression *expr,
    const Vector3f &position)
{
    changeExpression(expr);
    // move(position);
    setPosition(position);
}

void Character::exitStage()
{
    switchImage(1.0, "linear", { });
}
//
// void Character::pretendSay(
//     const std::string &fake_name,
//     const std::string &text)
// {
//     LOG(info, "{}({}): {}", fake_name, name(), text);
//     mLastName = fake_name;
//     mLastMessage = text;
//     sendEvent<CharacterMessageEvent>(this);
// }

void Character::exportScript(sol::table ns)
{
    ns.new_usertype<Character>(
        "Character",
        "enterStage", &Character::enterStage,
        "exitStage", &Character::exitStage,
        "changeExpression", &Character::changeExpression,
        "changePosition", &Character::changePosition,
        "say", &Character::say,
        "setAlias", &Character::setAlias,
        "removeAlias", &Character::removeAlias
    );
}
}
