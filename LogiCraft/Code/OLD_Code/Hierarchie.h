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

#pragma once
#include "SFML_ENGINE/WindowManager.h"
#include "SFML_ENGINE/RessourcesManager.h"

#include "tools_imgui.h"
#include "GameObject.h"
#include "Viewport.h"

constexpr unsigned short return_limit = 20;

class Hierarchie
{
public:
    Hierarchie();
    ~Hierarchie();

    void Update(std::shared_ptr<lc::GameObject> _scene, WindowManager& _window, Viewports& _viewports);

    ObjWeakPtrList& getSelectedGameObject();

private:
    /*
    * @brief This function is here to display all the Object in the world in the hierarchie window.
    */
    void GameObjectsDisplay(std::shared_ptr<lc::GameObject> _gameObject, std::shared_ptr<lc::GameObject> _scene,
                            std::list<std::shared_ptr<lc::GameObject>>* _gameObjectList);

    /*
    * @brief This function is here to display all the Object that as been selected in the selected object window.
    */
    void SelectedObjectsDisplay(std::shared_ptr<lc::GameObject> _scene, Viewports& _viewports);

    void add_component_to_object_window_display();

    void CreateNewObjectBehavior();

    /*
    * @brief A function to create a copy of the selected object when CTRL+C are press.
    * @brief Then it will be create when CTRL+V are press.
    */
    void CopyPaste(std::shared_ptr<lc::GameObject> _scene, Viewports& _viewport);

    void AddSelectedObject(std::shared_ptr<lc::GameObject> _object);

    void VerifyThePasteObject(std::shared_ptr<lc::GameObject>& _object);

    void MultipleSelection(std::shared_ptr<lc::GameObject>& _object);
    /*
    * @brief A function to display arrow to move up or down the object.
    * @brief To change the diplay order.
    * 
    * @param _gameObject : Actual Object that will be moved.
    * @param _gameObjectList : Is the object list of the parent of _gameObject to move _gameObject move up or down in it. 
    */
    void MoveDownUpBehavior(std::shared_ptr<lc::GameObject> _gameObject, ObjSharedPtrList* _gameObjectList);

    /*
    * @brief A function to move object in other object by drag it and drop it in the object.
    * 
    * @param _gameObject : The object that will be update to know if dragged or something need to be drop in.
    * @param _scene : The scene is here to know if the actual _gameObject is the scene so it can't be drag and drop.
    */
    void DragAndDropBehavior(std::shared_ptr<lc::GameObject> _gameObject, std::shared_ptr<lc::GameObject> _scene);

    /*
    * @brief A function to select a gameObject.
    * @brief by pressing CTRL+LeftClick the user can select multiple objects at once.
    * 
    * @param _gameObject : Its the object that is actually updated and if its selected or not.
    */
    void SelectionBehavior(std::shared_ptr<lc::GameObject> _gameObject);

    /*
    * @brief This function is used to know what input the user pressed and use to copy and paste,
    * @brief So if maj is press it will selected the parent and his child.
    * @brief And if ctrl is press it can multi select objects.
    */
    void SelectionVerifyBehavior(std::shared_ptr<lc::GameObject> _gameObject);

    /*
    * @brief A function to know if the _gameObject is a child of a selected object.
    * @brief Its to prevent double copy of an object.
    * 
    * @param _gameObject : The object that will be verified.
    * @param _scene : The object that is the WORLD its here to know if _gameObject is a child of it.
    */
    bool IsAChldOfASltObj(std::shared_ptr<lc::GameObject> _gameObject, std::shared_ptr<lc::GameObject> _scene);

    void setup_components_function();
    ObjWeakPtrList m_selectedGameObjects;
    ObjSharedPtrList m_copyPasteObjects;

    std::weak_ptr<lc::GameObject> m_clicked_object_;

    bool m_hasMoveAnObject;
    bool m_wantToCreateAGameObject;
    bool m_isDragging;
    bool m_isClicking;

    std::pair<bool, std::weak_ptr<lc::GameObject>> m_want_to_add_a_component_;
    std::map<std::string, std::function<void()>> m_components_map_;

    float m_copyPasteTimer;
};
