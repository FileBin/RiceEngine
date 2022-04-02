/*
 * ICleanable.hpp
 *
 *  Created on: 2 April 2022
 *      Author: FileBinsLapTop
 */

#include "../stdafx.hpp"

interface ICleanable;

#pragma once

interface ICleanable {
    virtual void cleanup() = 0;
    virtual ~ICleanable() {}
};
