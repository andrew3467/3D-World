//
// Created by apgra on 9/11/2023.
//

#include "Window.h"

#include <utility>

Window::Window(int width, int height, std::string title)
    : m_Width(width), m_Height(height), m_Title(std::move(title))
{   

}

Window::~Window() {

}
