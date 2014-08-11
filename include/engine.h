#ifndef ENGINE_H
#define ENGINE_H

/**
 * @brief Engine class
 * @details The core of the game engine.
 * 
 */
class Engine
{
 public:
 		/**
 		 * @brief Constructor
 		 * @details Default constructor
 		 */
 		Engine() { }

 		/**
 		 * @brief Destructor
 		 * @details Default destructor
 		 */
 		~Engine() { }

 		/**
 		 * @brief mainLoop method
 		 * @details Responsible for updating and drawing the objects.
 		 * Should be called only once.
 		 */
 		void MainLoop();
};

#endif //ENGINE_H