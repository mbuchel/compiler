//
// Created by Michael Buchel on 27/07/16.
//

#include <iostream>

#include "Error.h"

void Error::error(std::string message) {
    std::cout << message << std::endl;
    exit(EXIT_FAILURE);
}