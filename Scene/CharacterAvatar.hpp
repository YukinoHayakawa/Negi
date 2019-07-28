#pragma once

#include <Usagi/Math/Matrix.hpp>

#include "TransitionableImage.hpp"

namespace usagi::negi
{
class Expression;

class CharacterAvatar : public TransitionableImage
{
public:
    CharacterAvatar(Element *parent, std::string name);

    void hide();
    void change(Expression *expr);
    void setPosition(const Vector3f &position);
};
}
