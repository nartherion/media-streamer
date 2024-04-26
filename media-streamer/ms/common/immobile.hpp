#pragma once

namespace ms
{

class immobile
{
public:
    immobile() = default;

    immobile(const immobile &) = delete;
    immobile &operator=(const immobile &) = delete;

    immobile(immobile &&) = delete;
    immobile &operator=(immobile &&) = delete;
};

} // ms
