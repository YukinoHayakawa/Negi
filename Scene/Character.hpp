#pragma once

#include <Negi/Script/LuaForward.hpp>

#include <Usagi/Core/Element.hpp>
#include <Usagi/Math/Matrix.hpp>

#include "TransitionableImage.hpp"

namespace usagi::negi
{
class Expression;

class Character : public TransitionableImage
{
    bool mShowName = true;
    bool mShowAvatar = true;

    std::string mLastName;
    std::string mLastMessage;

    void setPosition(const Vector3f &position);

public:
    // todo load json config file
    Character(Element *parent, std::string name);

    void showName(bool show);
    void showAvatar(bool show);

    void enterStage(Expression *expr, const Vector3f &position);
    void exitStage();
    void changeExpression(Expression *expr);
    void changePosition(const Vector3f &position);
    void say(const std::string &text);
    void setAlias(std::string_view fake_name);
    void removeAlias();

    std::string lastName() const { return mLastName; }
    std::string lastMessage() const { return mLastMessage; }

    static void exportScript(sol::table ns);
};
}
