#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/random/mersenne_twister.hpp>

namespace storage_engine {

class UUIDGenerator {
public:
    static boost::uuids::uuid generateUUID() {
        // Create a Mersenne Twister random number generator
        boost::random::mt19937 rng;

        // Seed the random number generator with a time-based seed for better randomness
        boost::random::seed_seq seed{static_cast<unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count())};
        rng.seed(seed);

        // Create a UUID generator using the Mersenne Twister random number generator
        boost::uuids::random_generator<boost::random::mt19937> uuid_generator(rng);

        // Generate a UUID
        return uuid_generator();
    }
};

}