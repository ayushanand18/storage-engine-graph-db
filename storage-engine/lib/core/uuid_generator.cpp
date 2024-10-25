// core/uuid_generator.cpp
//
// Generate UUIDs for the storage engine.

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/random/mersenne_twister.hpp>

namespace storage_engine {

class UUIDGenerator {
public:

    static std::string generateUUID() noexcept {
        // Create a UUID generator
        boost::uuids::random_generator uuid_generator;

        // Generate a UUID
        boost::uuids::uuid uuid = uuid_generator();

        // Convert UUID to string and return
        return boost::lexical_cast<std::string>(uuid);
    }

};

}