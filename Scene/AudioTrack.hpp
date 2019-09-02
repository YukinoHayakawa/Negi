#pragma once

#include <Usagi/Core/PredefinedElement.hpp>
#include <Usagi/Extensions/SysBasicAudioMixing/BasicAudioStreamComponent.hpp>
#include <Negi/Script/LuaForward.hpp>

namespace usagi::negi
{
class Scene;

class AudioTrack : public PredefinedElement<BasicAudioStreamComponent>
{
    Scene *mScene = nullptr;

public:
    AudioTrack(Element *parent, std::string name, Scene *scene);

    void play(std::string_view asset_path, bool loop);
    void stop();

    static void exportScript(sol::table ns);
};
}
