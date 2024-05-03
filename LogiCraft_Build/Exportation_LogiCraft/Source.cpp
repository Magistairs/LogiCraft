#include "AI.h"
#include "GameObject.h"
#include "Tools_LC.h"
#include "SFML_ENGINE/WindowManager.h"
#include "Button.h"
#include "nfd.h"

std::shared_ptr<lc::GameObject> load()
{
	nfdchar_t* outPath = nullptr;
	nfdresult_t result = NFD_OpenDialog("lcg",nullptr, &outPath);
	fs::path path_ = (std::string(outPath));
	return lc::GameObject::LoadScene(path_.parent_path().filename().string());
}



int main()
{

	WindowManager window(1920, 1080, "test", false);
	lc::Button::SetupFunctionPool(window);

	auto world = load();
	auto player = lc::GameObject::CreateGameObject("Player", 8);
	const sf::Vector2f viewSize(1920 * 2, 1080 * 2);
	sf::View view(viewSize / 2.f, viewSize);
	window.getWindow().setView(view);
	sf::Vector2f Viewposition = viewSize/ 2.f;
	
	Tools::restartClock();
	while (!window.isDone())
	{
		Tools::restartClock();
		window.EventUpdate();

		world->Update(window);

		if (KEY(Add))
		{
			view.zoom(0.999f);
		}
		if (KEY(Subtract))
		{
			view.zoom(1.001f);
		}

		if(KEY(D))
			Viewposition.x += 1.f;
		if(KEY(Q))
			Viewposition.x -= 1.f;
		if(KEY(Z))
			Viewposition.y -= 1.f;
		if(KEY(S))
			Viewposition.y +=1.f;
		
		view.setCenter(Viewposition);

		window.getWindow().setView(view);
		window.clear(sf::Color::Black);
		world->Draw(window);
		window.display();
	}


	return 1;
}
