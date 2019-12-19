/*
 * TouchEventTransferAndroid.h
 *
 *  Created on: 2014年2月19日
 *      Author: mabin
 */

#ifndef TOUCHEVENTTRANSFERANDROID_H_
#define TOUCHEVENTTRANSFERANDROID_H_

#include <irrlicht.h>

namespace irr {
namespace android {

class TouchEventTransferAndroid {
public:
	TouchEventTransferAndroid();
	static bool OnTransferCommon(const SEvent& event);
private:
	static core::position2di m_pointer;
	static core::position2di m_down_pos;
	static core::position2di m_old_pointer;
};

} /* namespace android */
} /* namespace irr */
#endif /* TOUCHEVENTTRANSFERANDROID_H_ */
