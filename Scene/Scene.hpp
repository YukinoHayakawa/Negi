#pragma once

#include <Usagi/Core/Element.hpp>
#include <Usagi/Core/Math.hpp>

#include <Negi/JSON/JSONForwardDecl.hpp>
#include <Negi/Script/LuaForward.hpp>

namespace usagi::negi
{
class NegiGame;
class Expression;
class Character;
class ImageLayer;

class Scene : public Element
{
    NegiGame *mGame = nullptr;
    Element *mCharacters = nullptr;
    Element *mExpressions = nullptr;
    Element *mImageLayers = nullptr;

    AlignedBox3f mBound;
    std::map<std::string, Vector3f> mPositions;

    Character *mLastSpeakCharacter = nullptr;

public:
    Scene(Element *parent, std::string name, NegiGame *game);

    void load(const json &j);

    ImageLayer * createImageLayer(const std::string &name, float y_pos);
    Character * loadCharacter(const std::string &asset_locator);
    Expression * loadExpression(const std::string &name);
    Vector3f getPosition(const std::string &name) const;

    Character * lastSpeakCharacter() const { return mLastSpeakCharacter; }

    NegiGame * game() const { return mGame; }

    static void exportScript(sol::table ns);
};
}
