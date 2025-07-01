#ifndef EVENTS_H
#define EVENTS_H

/* Event ID codes. Sorted them to present local events first, but it is
 * just a 'cosmetic' thing.
 * Codes should not be changed, as they are linked to objects' subtype for handling.
 */

/**
 * @defgroup EVENT_xxx Events
 * To not have to check the whole inventory each time, each object has a
 * event_bitmask field which indicates whether the object has a handler
 * or not for this event. The bitmask is updated when an event hook is inserted
 * into another, and cleaned when a hook is removed from an object. In this
 * case is it recomputed the first time it is needed.
 * There are @subpage local_events and @subpage global_events.
 */

/** @defgroup local_events Local events
 * @ingroup EVENT_xxx
 * Local events are linked to particular objects, for example, a special apply
 * event for a particular artifact sword. These objects need to contain a event
 * handler object specifying the plugin/module to run in order for these types
 * of events to be dispatched.
 * @{
 */
#define EVENT_NONE      0  /**< No event. This exists only to reserve the "0". */
#define EVENT_APPLY     1  /**< Object applied-unapplied.                      */
#define EVENT_ATTACKED  2  /**< Object attacked, with weapon or spell.         */
#define EVENT_ATTACKS   33 /**< Weapon or arrow hitting something.             */
#define EVENT_BOUGHT    34 /**< Object is being bought by player.              */
#define EVENT_CLOSE     11 /**< Container closed.                              */
#define EVENT_DEATH     3  /**< Player or monster dead.                        */
#define EVENT_DESTROY   13 /**< Object destroyed (includes map reset/swapout)  */
#define EVENT_DROP      4  /**< Object dropped on the floor.                   */
#define EVENT_PICKUP    5  /**< Object picked up.                              */
#define EVENT_SAY       6  /**< Someone speaks.                                */
#define EVENT_SELLING   32 /**< Object is being sold by another one.           */
#define EVENT_STOP      7  /**< Thrown object stopped.                         */
#define EVENT_TIME      8  /**< Triggered each time the object can react/move. */
#define EVENT_THROW     9  /**< Object is thrown.                              */
#define EVENT_TRIGGER   10 /**< Button pushed, lever pulled, etc.              */
#define EVENT_TIMER     12 /**< Timer connected triggered it.                  */
#define EVENT_USER      31 /**< User-defined event.                            */
/**@}*/

/** @defgroup global_events Global events
 * @ingroup EVENT_xxx
 * Global events dispatch to all plugins that register to the event.
 * @{
 */
#define EVENT_BORN      14 /**< A new character has been created.              */
#define EVENT_CLOCK     15 /**< Global time event.                             */
#define EVENT_CRASH     16 /**< Triggered when the server crashes. Not recursive */
#define EVENT_GKILL     18 /**< Triggered when anything got killed by anyone.  */
#define EVENT_KICK      28 /**< A player was Kicked by a DM                    */
#define EVENT_LOGIN     19 /**< Player login.                                  */
#define EVENT_LOGOUT    20 /**< Player logout.                                 */
#define EVENT_MAPENTER  21 /**< A player entered a map.                        */
#define EVENT_MAPLEAVE  22 /**< A player left a map.                           */
#define EVENT_MAPLOAD   30 /**< A map is loaded (pristine state)               */
#define EVENT_MAPREADY  35 /**< A map is ready, either first load or after reload */
#define EVENT_MAPRESET  23 /**< A map is resetting.                            */
#define EVENT_MAPUNLOAD 29 /**< A map is freed (includes swapping out)         */
#define EVENT_MUZZLE    27 /**< A player was Muzzled (no_shout set).           */
#define EVENT_PLAYER_DEATH  17 /**< Global Death event                         */
#define EVENT_REMOVE    24 /**< A Player character has been removed.           */
#define EVENT_SHOUT     25 /**< A player 'shout' something.                    */
#define EVENT_TELL      26 /**< A player 'tell' something.                     */
#define EVENT_GBOUGHT   36 /**< Player bought object in shop, but global.      */
#define EVENT_GSOLD     37 /**< Player sold object in shop, but global.        */
/**@}*/

#define NR_EVENTS 38  /**< Number of events, maximum code + 1. */

/** Convert an event to its bit. */
#define BITMASK_EVENT(evt)  (1ULL << evt)
/** Bit indicating if the event bitmask is valid or not. */
#define BITMASK_VALID       (1ULL << 63)

/** Function to call to handle global or object-related events. */
typedef int (*f_plug_event)(int *type, ...);
/** Registration identifier type. */
typedef unsigned long event_registration;

/**
 * Register a global event handler.
 * @param eventcode @ref EVENT_xxx "event code" to register the handler for.
 * @param hook function to call for the event.
 * @return registration identifier, used with events_unregister_global_handler().
 */
event_registration events_register_global_handler(int eventcode, f_plug_event hook);

/**
 * Remove a global event handler.
 * @param eventcode @ref EVENT_xxx "event code" to unregister the handler of.
 * @param id registration identifier as returned by events_register_global_handler().
 */
void events_unregister_global_handler(int eventcode, event_registration id);

/**
 * Execute a global event.
 * @param eventcode @ref EVENT_xxx "event code".
 * @param ... additional parameters, depending on the event.
 */
void events_execute_global_event(int eventcode, ...);

/**
 * Register an object event handler.
 * @param id handler identifier, must be unique and match the "title" field of the event connector.
 * @param handler function to call for the event.
 */
void events_register_object_handler(const char *id, f_plug_event handler);

/**
 * Remove an object event handler.
 * @param id handler identifier.
 */
void events_unregister_object_handler(const char *id);

/**
 * Execute an ::EVENT_USER on the specified object.
 * @param op object to execute the event on.
 * @param activator object which caused the event to be triggered.
 * @param third additional object.
 * @param message what the object said to trigger.
 * @param fix whether to fix op or not.
 * @return value to return to the caller.
 */
int events_execute_object_user(object *op, object *activator, object *third, const char *message, int fix);

/**
 * Execute an event on the specified object.
 * @param op object to execute the event on.
 * @param eventcode @ref EVENT_xxx "event code" to execute.
 * @param activator object which caused the event to be triggered.
 * @param third additional object.
 * @param message what the object said to trigger.
 * @param fix whether to fix op or not.
 * @return value to return to the caller.
 */
int events_execute_object_event(object *op, int eventcode, object *activator, object *third, const char *message, int fix);

/**
 * Execute an ::EVENT_SAY on the specified object.
 * @param npc object to execute the event on.
 * @param talk dialog information, which may be altered to add replies or questions.
 * @return value to return to the caller.
 */
int events_execute_object_say(object *npc, talk_info *talk);

#endif /* EVENTS_H */
