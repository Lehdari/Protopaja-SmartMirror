// vision.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "vision.h"
#include "Canvas.h"
//#include "Texture.h"
//#include "Shader.h"


#include <SFML/Window.hpp>

#include <iostream>


#define QUAD(xPos, yPos, xSize, ySize) xPos, yPos, 0.0f,\
                                       xPos+xSize, yPos, 0.0f,\
                                       xPos, yPos+ySize, 0.0f,\
                                       xPos, yPos+ySize, 0.0f,\
                                       xPos+xSize, yPos, 0.0f,\
                                       xPos+xSize, yPos+ySize, 0.0f



//using namespace cv;
using namespace std;


namespace Vision
{

	int testMain(void) {		
		sf::Window window(sf::VideoMode(1920, 540), "Vision", sf::Style::Default,
		                  sf::ContextSettings{ 24, 8, 4, 3, 0 });
		window.setFramerateLimit(30);
		window.setActive();

		if (glewInit() != GLEW_OK) {
			fprintf(stderr, "Unable to init GLEW");
			return -1;
		}

		glEnable(GL_TEXTURE_2D);

		Canvas canvas;

		while (window.isOpen())
		{
			// Event processing
			sf::Event event;
			while (window.pollEvent(event))
			{
				// Request for closing the window
				if (event.type == sf::Event::Closed)
					window.close();
			}

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			canvas.draw();

			window.display();
		}

		return 0;
	}

}