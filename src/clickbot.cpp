#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/FMODAudioEngine.hpp>
#include <zBot.hpp>

void setupFolder(std::string path) {
    auto dir = Mod::get()->getSaveDir() / path;
    
    if (ghc::filesystem::is_directory(dir)) {
        for (auto& entry : ghc::filesystem::directory_iterator(dir)) {
            auto file = entry.path();
            if (file.extension() == ".wav") return;
        }
    }

    auto fileName = Mod::get()->getResourcesDir() / (path + ".zip");
    file::Unzip::intoDir(fileName, dir, false);
}

void loadClicks(std::string folder, std::vector<FMOD::Sound*>* vec, FMOD::System* sys) {
    setupFolder(folder);
    
    auto dir = Mod::get()->getSaveDir() / folder;
    for (auto& entry : ghc::filesystem::directory_iterator(dir)) {
        auto file = entry.path();
        if (file.extension() != ".wav") continue;
            
        FMOD::Sound* sound;
        sys->createSound(file.string().c_str(), FMOD_LOOP_OFF, 0, &sound);
        vec->push_back(sound);
    }
}

using namespace geode::prelude;

$execute {
    Mod::get()->patch(
        reinterpret_cast<void*>(geode::base::get() + GEODE_WINDOWS(0x23afe) GEODE_MACOS(0x20f140)), 
        toByteArray((INT8) FMOD_MAX_CHANNEL_WIDTH)
    );
}

class $modify(PlayLayer) {
    std::vector<FMOD::Sound*> clicks;
    std::vector<FMOD::Sound*> releases;
    std::vector<FMOD::Sound*> softClicks;
    std::vector<FMOD::Sound*> softReleases;

    FMOD::ChannelGroup* clickbotChannelGroup;

    int lastFrame = -1;

    bool state = false;

    bool init(GJGameLevel* lvl) {
        FMOD::System* sys = FMODAudioEngine::sharedEngine()->m_system;
        
        auto dir = Mod::get()->getSaveDir();

        loadClicks("clicks", &m_fields->clicks, sys);
        loadClicks("releases", &m_fields->releases, sys);
        loadClicks("softClicks", &m_fields->softClicks, sys);
        loadClicks("softReleases", &m_fields->softReleases, sys);

        sys->createChannelGroup("zBot Clicks", &m_fields->clickbotChannelGroup);
        m_fields->clickbotChannelGroup->setVolume(10.f);
        
        return PlayLayer::init(lvl);
    }

    void playSound(bool click) {
        FMODAudioEngine::sharedEngine()->m_globalChannel->setPriority(0);
        if (m_fields->state != click) {
            m_fields->state = click;

            float bias = 0.8 + static_cast<float>(rand())/(static_cast<float>(RAND_MAX/(1.f-0.8)));
            float mult = 1.f;

            float delta = m_fields->lastFrame != -1 ? (zBot::get()->frame - m_fields->lastFrame) * zBot::get()->currentReplay->delta : 5;

            if (click) {
                if (delta <= 0.2) {
                    mult *= 20.f * delta * delta + 0.0613;
                }
                m_fields->lastFrame = zBot::get()->frame;
            };

            std::vector<FMOD::Sound*> group;

            if (delta <= 0.2) group = click ? m_fields->softClicks : m_fields->softReleases;
            else group = click ? m_fields->clicks : m_fields->releases;

            FMOD::ChannelGroup* channelGroup = m_fields->clickbotChannelGroup;

            FMOD::Channel* soundChannel;
            FMODAudioEngine::sharedEngine()->m_system->playSound(group[std::rand() % group.size()], channelGroup, false, &soundChannel);

            soundChannel->setVolume(bias * mult);
            soundChannel->setPriority(256);
        }
    }

    void resetLevel() {
        m_fields->lastFrame = -1;
        PlayLayer::resetLevel();
    }

    void update(float delta) {
        zBot* mgr = zBot::get();

        if (mgr->state == PLAYBACK) {
            int clickOffset = 0.1 / mgr->currentReplay->delta;
            auto p1_inputs = mgr->currentReplay->p1_inputs.find(mgr->frame + clickOffset);
            auto p2_inputs = mgr->currentReplay->p2_inputs.find(mgr->frame + clickOffset);

            if (p1_inputs != mgr->currentReplay->p1_inputs.end()) {
                for (bool click : p1_inputs->second) {
                    playSound(click);
                }
            }

            if (p2_inputs != mgr->currentReplay->p2_inputs.end()) {
                for (bool click : p2_inputs->second) {
                    playSound(click);
                }
            }
        }

        return PlayLayer::update(delta);
    }
};