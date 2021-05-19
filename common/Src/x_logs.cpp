/*
 * x_logs.cpp
 *
 *  Created on: May 19, 2021
 *      Author: xrj
 */

#include "x_logs.hpp"

const string bulid_date[12] = __DATE__;
const string bulid_time[9] = __TIME__;

//use X_Error(__FILE__, __LINE__);
void X_Error(string file, int line)
{
    while(1);
}
