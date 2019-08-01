#pragma once

#include <Usagi/Core/Element.hpp>
#include <Usagi/Math/Matrix.hpp>
#include <Negi/Script/LuaForward.hpp>
#include <Negi/JSON/JSONForwardDecl.hpp>

#include "TransitionableImage.hpp"

namespace usagi::negi
{
class Expression;

class Character : public TransitionableImage
{
    bool mShowName = true;
    bool mShowAvatar = true;
    Expression *mCurrentExpression = nullptr;

    std::string mLastName;
    std::string mLastMessage;

    void setPosition(const Vector3f &position);

public:
    // todo load json config file
    Character(Element *parent, std::string name);

    void load(const json &j);

    bool showName() const
    {
        return mShowName;
    }

    void setShowName(bool show_name)
    {
        mShowName = show_name;
    }

    bool showAvatar() const
    {
        return mShowAvatar;
    }

    void setShowAvatar(bool show_avatar)
    {
        mShowAvatar = show_avatar;
    }

    Expression * currentExpression() const
    {
        return mCurrentExpression;
    }

    void enterStage(Expression *expr, const Vector3f &position);
    void exitStage();
    void changeExpression(Expression *expr);
    void changePosition(const Vector3f &position);
    void say(const std::string &text);
    void setAlias(std::string_view fake_name);
    void removeAlias();

    std::string_view lastName() const { return mLastName; }
    std::string_view lastMessage() const { return mLastMessage; }

    static void exportScript(sol::table ns);
};
}
