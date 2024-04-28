#pragma once

namespace ms::common
{

class immobile
{
public:
    immobile() = default;

    immobile(const immobile &instance) = delete;
    immobile &operator=(const immobile &instance) = delete;

    immobile(immobile &&instance) = delete;
    immobile &operator=(immobile &&instance) = delete;
};

} // ms::common
