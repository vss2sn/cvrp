/**
 * @file graphic_utils.hpp
 * @author vss2sn
 * @brief Contains the functions and utils needed to display the solution using
 * SFML
 */

#ifndef GRAPHIC_UTILS_HPP
#define GRAPHIC_UTILS_HPP

#include "cvrp/utils.hpp"
/**
 * @brief Displays the solution passed in  by plotting the routes
 * @param s solution to be displayed.
 * @return void
 */
void DisplaySolution(const Solution& s);

#endif  // GRAPHIC_UTILS_HPP
