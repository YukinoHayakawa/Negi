#pragma once

#include <Usagi/Graphics/Game/GraphicalGameState.hpp>

namespace usagi::negi
{
class NegiGame;

class NegiGameState : public GraphicalGameState
{
public:
    NegiGameState(Element *parent, std::string name, NegiGame *game);

    NegiGame * game() const;
};
}
