/*implementation from https://github.com/minetest/minetest/blob/02a23892f94d3c83a6bdc301defc0e7ade7e1c2b/src/gui/modalMenu.cpp#L116
with this patch applied to the engine https://github.com/minetest/minetest/blob/02a23892f94d3c83a6bdc301defc0e7ade7e1c2b/build/android/patches/irrlicht-touchcount.patch
*/

#include "TouchEventTransferAndroid.h"
#include "../gframe/game.h"

using namespace irr;

namespace irr {
namespace android {
core::position2di TouchEventTransferAndroid::m_pointer;
core::position2di TouchEventTransferAndroid::m_down_pos;
core::position2di TouchEventTransferAndroid::m_old_pointer;

TouchEventTransferAndroid::TouchEventTransferAndroid() {}

bool TouchEventTransferAndroid::OnTransferCommon(const SEvent& event) {
	if(event.EventType == EET_TOUCH_INPUT_EVENT) {
		SEvent translated;
		memset(&translated, 0, sizeof(SEvent));
		translated.EventType = EET_MOUSE_INPUT_EVENT;

		translated.MouseInput.X = event.TouchInput.X;
		translated.MouseInput.Y = event.TouchInput.Y;
		translated.MouseInput.Control = false;

		bool dont_send_event = false;

		if(event.TouchInput.touchedCount == 1) {
			switch(event.TouchInput.Event) {
				case ETIE_PRESSED_DOWN:
					m_pointer = core::position2di( event.TouchInput.X, event.TouchInput.Y);
					translated.MouseInput.Event = EMIE_LMOUSE_PRESSED_DOWN;
					translated.MouseInput.ButtonStates = EMBSM_LEFT;
					m_down_pos = m_pointer;
					SEvent hoverEvent;
					hoverEvent.EventType = EET_MOUSE_INPUT_EVENT;
					hoverEvent.MouseInput.Event = EMIE_MOUSE_MOVED;
					hoverEvent.MouseInput.X = event.TouchInput.X;
					hoverEvent.MouseInput.Y = event.TouchInput.Y;
					ygo::mainGame->device->postEventFromUser(hoverEvent);
					break;
				case ETIE_MOVED:
					m_pointer = core::position2di( event.TouchInput.X, event.TouchInput.Y);
					translated.MouseInput.Event = EMIE_MOUSE_MOVED;
					translated.MouseInput.ButtonStates = EMBSM_LEFT;
					break;
				case ETIE_LEFT_UP:
					translated.MouseInput.Event = EMIE_LMOUSE_LEFT_UP;
					translated.MouseInput.ButtonStates = 0;
					// we don't have a valid pointer element use last
					// known pointer pos
					translated.MouseInput.X = m_pointer.X;
					translated.MouseInput.Y = m_pointer.Y;

					// reset down pos
					m_down_pos = core::position2di( 0, 0);
					break;
				default:
					dont_send_event = true;
					// this is not supposed to happen
					//errorstream << "GUIModalMenu::preprocessEvent"
					//	<< " unexpected usecase Event="
					//	<< event.TouchInput.Event << std::endl;
			}
		} else if((event.TouchInput.touchedCount == 2) &&
			(event.TouchInput.Event == ETIE_PRESSED_DOWN)) {

			translated.MouseInput.Event = EMIE_RMOUSE_PRESSED_DOWN;
			translated.MouseInput.ButtonStates = EMBSM_LEFT | EMBSM_RIGHT;
			translated.MouseInput.X = m_pointer.X;
			translated.MouseInput.Y = m_pointer.Y;
			ygo::mainGame->device->postEventFromUser(translated);

			translated.MouseInput.Event = EMIE_RMOUSE_LEFT_UP;
			translated.MouseInput.ButtonStates = EMBSM_LEFT;

			ygo::mainGame->device->postEventFromUser(translated);
			dont_send_event = true;
		}
		// ignore unhandled 2 touch events ... accidental moving for example
		else if(event.TouchInput.touchedCount == 2) {
			dont_send_event = true;
		} else if(event.TouchInput.touchedCount > 2) {
			//errorstream << "GUIModalMenu::preprocessEvent"
			//	<< " to many multitouch events "
			//	<< event.TouchInput.touchedCount << " ignoring them"
			//	<< std::endl;
		}

		if(dont_send_event) {
			return true;
		}

		// check if translated event needs to be preprocessed again
		if(OnTransferCommon(translated)) {
			return true;
		}
		
		bool retval = ygo::mainGame->device->postEventFromUser(translated);;

		if(event.TouchInput.Event == ETIE_LEFT_UP) {
			// reset pointer
			m_pointer = core::position2di( 0, 0);
		}
		return retval;
	}
	return false;
}
} /* namespace android */
} /* namespace irr */
