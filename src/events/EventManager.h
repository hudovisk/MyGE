
#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include "fastDelegate/delegate.hpp"
#include "events/Events.h"
#include "events/InputTypes.h"

#include "SDL2/SDL.h"

#include <map>
#include <list>
#include <functional>

typedef srutil::delegate1<void , IEventDataPtr> EventListenerDelegate;

/**
 * @brief EventManager sub system
 * @details Responsible for initialise event modules of SDL (SDL must be already initialise by System class).
 * Responsible for polling events of SDL and translate them for the engine especific classes. It dispatches events
 * for all registered listenners.
 */
class EventManager
{
public:
	/**
	 * @brief Default constructor
	 */
	EventManager();

	/**
	 * @brief Initialise method.
	 * @details Initialise SDL_INIT_EVENTS & SDL_INIT_JOYSTICK sub systems, also initialise the map structures
	 * used to translate SDL_Events to InputEvent.
	 * @return true if success, false otherwise.
	 */
	bool init();

	/**
	 * @brief Cleanup method
	 * @details Quit the SDL sub systems initialised in init()
	 * @return true if success, false otherwise.
	 */
	bool destroy();

	/**
	 * @brief Dispatch all queue events.
	 * @details Responsible for poll events of SDL and translate them, the events can be accessed by getInputQueue().
	 * It iterates over all event queued by queueEvent() and notify all listenners registered.
	 * @return true if success, false otherwise.
	 */
	bool dispatchEvents();

	/**
	 * @brief Registers a listenner for a specific event type.
	 * @details Register a listenner that will be called by dispatchEvents or triggerEvent if the type parameter is the same of the 
	 * event type queued/triggered.
	 * 
	 * @param eventDelegate The callback method that will be called by dispatchEvent or triggerEvent.
	 * @param int The type of event. Use the getType() method of the desired EventData.
	 * 
	 * @return true if success, false otherwise.
	 */
	bool addListenner(const EventListenerDelegate& eventDelegate, const unsigned int type);

	/**
	 * @brief Remove the listenner registered to the event type specified in the parameters.
	 * 
	 * @param eventDelegate The callback method already registered.
	 * @param int The type of event. Use the getType() method of the desired EventData.
	 * 
	 * @return true if success, false otherwise.
	 */
	bool removeListenner(const EventListenerDelegate& eventDelegate, const unsigned int type);


	/**
	 * @brief Trigger all the listenners registered to the desired event type.
	 * @details Holds execution, iterating over all listenners of event type and notifing each.
	 * 
	 * @param event Pointer to event data.
	 * @return true if success, false otherwise.
	 */
	bool triggerEvent(const IEventDataPtr event) const;

	/**
	 * @brief Queue event.
	 * @details Queue event that will be dispatched to all listenners when dispatchEvents is called.
	 * 
	 * @param event Pointer to event data.
	 * @return true if success, false otherwise.
	 */
	bool queueEvent(const IEventDataPtr event);

	/**
	 * @brief Remove all events queued of the same type as specified by parameter type.
	 * 
	 * @param int The type of event. Use the getType() method of the desired EventData.
	 * @return true if success, false otherwise.
	 */
	bool abortEvent(const unsigned int type);

	/**
	 * @brief Get all translated input events by dispatchEvents.
	 * @details The inputQueue is not consumed by default. If size > MAX_INPUT_SIZE older InputEvents will be replaced by newer ones.
	 * To consume an InputEvent just remove it from the returned list.
	 * @return List containing all translated InputEvent in the same order as retrivied by SDL_PollEvent.
	 */
	std::list<InputEvent>& getInputQueue() { return m_inputQueue; }

private:

	void translateSDLEvent(const SDL_Event& event);

	KeyboardEvent mapKeyInput(const SDL_Event& key) const;
	MouseEvent mapMouseInput(const SDL_Event& mouse) const;

	using DelegateList = std::list<EventListenerDelegate>;
	using EventListennersMap = std::map<unsigned int, DelegateList>;
	using GetIEventDataPtr = std::function<IEventDataPtr(const SDL_Event& event)>;

	const unsigned int MAX_INPUT_SIZE = 100;

	/*
	 * Maps
	 */
	EventListennersMap m_mapEventListeners;
	std::map<unsigned int, GetIEventDataPtr> m_eventMap;
	std::map<unsigned int, InputEventType> m_inputEventMap;

	/*
	 * Queues
	 */
	std::list<InputEvent> m_inputQueue;
	std::list<IEventDataPtr> m_eventQueue;
};

#endif 