#ifndef _replay_hpp
#define _replay_hpp

#include <Geode/Geode.hpp>
#include <vector>

class Replay {
public:
    std::map<int, std::vector<bool>> p1_inputs;
    std::map<int, std::vector<bool>> p2_inputs;
    float delta;
    std::string name;
    float version;

    void save() {
        ghc::filesystem::ofstream myfile;
        
        auto dir = geode::prelude::Mod::get()->getSaveDir() / "replays";
        if (ghc::filesystem::exists(dir) || ghc::filesystem::create_directory(dir)) {
            myfile.open(dir  / (name + ".zbf"), std::ios::binary);

            myfile.write(reinterpret_cast<const char*>(&delta), sizeof(float));
            myfile.write(reinterpret_cast<const char*>(&version), sizeof(float));
            
            for (auto it = p1_inputs.begin(); it != p1_inputs.end(); it++) {
                for (auto el : it->second) {
                    myfile.write(reinterpret_cast<const char*>(&it->first), sizeof(int));
                    myfile << el << true;
                }
            }
            
            for (auto it = p2_inputs.begin(); it != p2_inputs.end(); it++) {
                for (auto el : it->second) {
                    myfile.write(reinterpret_cast<const char*>(&it->first), sizeof(int));
                    myfile << el << false;
                }
            }

            myfile.close();
        }
    }

    static Replay* fromFile(std::string filename) {
        auto dir = geode::prelude::Mod::get()->getSaveDir() / "replays";
        if (ghc::filesystem::exists(dir) || ghc::filesystem::create_directory(dir)) {
            ghc::filesystem::ifstream infile(dir / (filename + ".zbf"), std::ios::binary);
            if (!infile.is_open())
            {
                return nullptr;
            }

            Replay* rec = new Replay();

            char* dR = new char[sizeof(float) + 1];

            infile.read(dR, sizeof(float));

            rec->delta = *reinterpret_cast<float*>(dR);

            infile.read(dR, sizeof(float));

            rec->version = *reinterpret_cast<float*>(dR);

            while (infile.peek() != EOF) {
                infile.read(dR, sizeof(float));
                int frame = *reinterpret_cast<int*>(dR);

                char str[3];

                infile.read(str, 2);
                bool click = str[0] != '0';

                bool p1 = str[1] != '0';

                if (p1) {
                    rec->p1_inputs[frame].push_back(click);
                } else {
                    rec->p2_inputs[frame].push_back(click);
                }
            }


            infile.close();

            auto pos = filename.rfind(".zbf");
            if (pos != std::string::npos) {
                filename.erase(pos);
            }

            rec->name = filename;

            return rec;
        }
        return nullptr;
    }

    void addInput(int frame, bool click, bool p1) {
        if (p1) p1_inputs[frame].push_back(click);
        else p2_inputs[frame].push_back(click);
    }

    void purgeInputs(int frame) {
        for (auto it = p1_inputs.begin(); it != p1_inputs.end(); it++) {
            if (it->first >= frame) {
                p1_inputs.erase(it, p1_inputs.end());
                break;
            }
        }

        for (auto it = p2_inputs.begin(); it != p2_inputs.end(); it++) {
            if (it->first >= frame) {
                p2_inputs.erase(it, p2_inputs.end());
                break;
            }
        }
    }
};

#endif