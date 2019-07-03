#pragma once

#include <sol/forward.hpp>

#include <Usagi/Core/Element.hpp>
#include <Usagi/Core/Math.hpp>

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

    void enterScene(Expression *expr, const Vector3f &position);
    void exitScene();
    void changeExpression(Expression *expr);
    void move(const Vector3f &position);
    void say(const std::string &text);
    void pretendSay(const std::string &fake_name, const std::string &text);

    std::string lastName() const { return mLastName; }
    std::string lastMessage() const { return mLastMessage; }

    static void exportScript(sol::table ns);
};
}
