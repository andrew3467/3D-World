//
// Created by apgra on 9/11/2023.
//

#ifndef APPLICATION_WINDOW_H
#define APPLICATION_WINDOW_H


#include <string>

class Window {
private:
    unsigned int m_ID;

    int m_Width, m_Height;
    std::string m_Title;


public:
    Window(int width, int height, std::string title);
    ~Window();

    Window(Window& other) = delete;
};


#endif //APPLICATION_WINDOW_H
