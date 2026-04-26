#include "EngineApp.h"
#include "utility/PlatformWindowInfo_Conversion.h"

namespace CSEEngine
{

static void LogKeyInput(CSEApplication::KeyType keyType, CSEApplication::KeyPressState keyPressState)
{
	std::string keyString = CSEApplication::KeyTypeToString(keyType);
	if (keyPressState == CSEApplication::KEY_DOWN)
	{
		keyString += " key down.";
	}
	else if (keyPressState == CSEApplication::KEY_HELD)
	{
		keyString += " key held.";
	}
	else if (keyPressState == CSEApplication::KEY_UP)
	{
		keyString += " key up.";
	}

	CSE_LOGI(keyString);
}
class InputDumper
{
public:
	InputDumper()
		: _inputQueue()
	{
		CSECore::EventCallback<CSEApplication::InputEvent> inputCallback;
		inputCallback.BindCallback<InputDumper, &InputDumper::HandleInputEvent>(this);

		CSECore::EventCallbackList<CSEApplication::InputEvent> inputCallbackList;
		inputCallbackList.AddCallback(CSEApplication::INPUT_TYPE_KEYBOARD, inputCallback);

		_inputQueue.BindCallbackList(inputCallbackList);

		CSEApplication::InputManager::AddInputEventListener(CSEApplication::INPUT_TYPE_KEYBOARD, &_inputQueue);
	}

	~InputDumper()
	{
		CSEApplication::InputManager::RemoveInputEventListener(CSEApplication::INPUT_TYPE_KEYBOARD, &_inputQueue);
		_inputQueue.ClearCallbackList();
	}

	void Update()
	{
		_inputQueue.Process();
	}

	void HandleInputEvent(CSEApplication::InputEvent input)
	{
		if (input.type == CSEApplication::INPUT_TYPE_KEYBOARD)
		{
			LogKeyInput(input.eventUnion.keyboard.keyType, input.eventUnion.keyboard.keyPressState);
		}
	}

private:
	CSECore::EventQueue<CSEApplication::InputEvent> _inputQueue;
};

EngineApp::EngineApp()
	: _renderContext()
{

}

EngineApp::~EngineApp()
{

}

void EngineApp::Initialize(std::string name)
{
	App::Initialize(name);
	_renderContext.Initialize();
	CSECore::Any<64> winInfoApp = GetWindow()->GetPlatformWindowInfo();
	CSECore::Any<64> winInfoRenderer = PlatformWindowInfo_AppToRenderer(winInfoApp);
	_renderContext.GetBackend()->SetTargetWindow(winInfoRenderer);
}

void EngineApp::Run()
{
	InputDumper inputDumper;

	// do time step stuff + tick + frame etc.
	while (!ShouldQuit())
	{
		PollPlatformEvents();
		UpdateInputManager();
		inputDumper.Update();
	}
}

void EngineApp::Dispose()
{

	_renderContext.Dispose();
	App::Dispose();
}

}