#include <string>

namespace Wisenet
{
namespace Library
{

class SecurityManager{
public:
    static std::string EncryptPassword(std::string publicKey, std::string password);
};

}
}
