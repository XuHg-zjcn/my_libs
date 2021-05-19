/*
 * ops.h
 *
 *  Created on: Apr 26, 2021
 *      Author: xrj
 */

#ifndef INC_OPS_H_
#define INC_OPS_H_

//a,b,c const, x,y,z vars

#define value_lower(x, a)   (x<a)?a:x
#define value_upper(x, b)   (x>b)?b:x
#define value_clip(x, a, b) (x<a)?a:(x>b?b:x)

#define swap(x, y)          {x=x^y;y=y^x;x=x^y;}

#endif /* INC_OPS_H_ */
