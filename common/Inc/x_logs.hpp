/*
 * x_logs.h
 *
 *  Created on: May 19, 2021
 *      Author: xrj
 */

#ifndef COMMON_INC_X_LOGS_HPP_
#define COMMON_INC_X_LOGS_HPP_

#include <string>
using namespace std;

#ifdef __cplusplus
extern "C" {
#endif

void X_ErrorLog(string _file, int _line);

#ifdef __cplusplus
}
#endif


#endif /* COMMON_INC_X_LOGS_HPP_ */
