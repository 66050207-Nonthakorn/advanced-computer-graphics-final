#pragma once

#include <memory>
#include <string>

class Texture {    
public:
    virtual ~Texture() = default;

    void bind(int slot) const;
    unsigned int getId() const;
    virtual int getType() const = 0;

protected:
    unsigned int id;
};