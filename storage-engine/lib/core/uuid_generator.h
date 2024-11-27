#pragma once

#ifndef CORE_UUID_GENERATOR_H
#define CORE_UUID_GENERATOR_H

#include <string>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/lexical_cast.hpp>

namespace storage_engine {

class UUIDGenerator {
public:
    static std::string generateUUID() noexcept;
};

} // namespace storage_engine

#endif // CORE_UUID_GENERATOR_H
