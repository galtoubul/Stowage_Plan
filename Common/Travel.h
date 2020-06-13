/**
 * Class for the representation of Travel
 */

#include <filesystem>
#include <string>
namespace fs = std::filesystem;
class Travel {
    int index;
    fs::path shipPlanPath;
    fs::path shipRoutePath;
    fs::path dir;
    std::string output;
    std::string name;

public:
    Travel(int _index, fs::path _shipPlanPath, fs::path _shipRoutePath, fs::path _dir, const std::string& _output, const std::string& _name) :
    index(_index), shipPlanPath(_shipPlanPath), shipRoutePath(_shipRoutePath), dir(_dir), output(_output), name(_name) {}

    // ------------------------- getters ------------------------ //
    int getIndex();
    fs::path& getShipPlanPath();
    fs::path& getShipRoutePath();
    fs::path& getDir();
    std::string getOutputPath();
    std::string getName();
};

