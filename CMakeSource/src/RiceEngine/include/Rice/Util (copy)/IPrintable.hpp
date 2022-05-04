/*
 * IPrintable.hpp
 *
 *  Created on: 31 March 2022
 *      Author: FileBinsLapTop
 */

#include "../stdafx.hpp"

interface IPrintable;
struct String;

#pragma once

interface IPrintable {
    virtual String toString() = 0;
    virtual ~IPrintable() {}
};
