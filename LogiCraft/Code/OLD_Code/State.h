
/*------------------------------------LICENSE------------------------------------
MIT License

Copyright (c) [2024] [GRALLAN Yann]
Copyright (c) [2024] [LESAGE Charles]
Copyright (c) [2024] [MENA-BOUR Samy]

This software utilizes code from the following GitHub repositories, which are also licensed under the MIT License:

- [SFML](https://github.com/SFML)
- [ImGUI](https://github.com/ocornut/imgui)
- [ImNodes](https://github.com/Nelarius/imnodes)
- [SFML-Manager](https://github.com/Xanhos/Cpp-Manager-for-SFML)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
---------------------------------------------------------------------------------*/

#include "SFML_ENGINE/WindowManager.h"
#include "SFML_ENGINE/RessourcesManager.h"

class State
{
protected:
    std::list<std::unique_ptr<State>>* m_listState;
    WindowManager& m_windowManager;
    bool m_isReady;
    bool m_initIsStarted;
    bool m_needToBeDeleted;
    std::string m_name;
public:
    State(WindowManager& _window, std::list<std::unique_ptr<State>>* listState);
    virtual ~State() {};

    template <typename T>
    static std::unique_ptr<T> Create(WindowManager& _window, std::list<std::unique_ptr<State>>* listState)
    {
		return std::make_unique<T>(_window, listState);
	}

    bool getIsReady() { return m_isReady; }
    bool& getNeedToBeDeleted() { return m_needToBeDeleted; }
    bool& getIsStarted() { return m_initIsStarted; }
    void virtual init() = 0;
    void virtual updateEvent(sf::Event& _event) = 0;
    void virtual update() = 0;
    void virtual render() = 0;
    void virtual pushState(char data) = 0;
    std::string virtual getName() { return m_name; }
};

typedef std::list<std::unique_ptr<State>> StateList;