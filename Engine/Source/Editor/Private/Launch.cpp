#include "pch.h"

#include "Renderwerk/Platform/Launch.h"

#include "Renderwerk/Application/Application.h"

class FEditorApplication : public IApplication
{
public:
	explicit FEditorApplication()
		: IApplication({.Name = "Editor", .Version = RW_MAKE_VERSION(1, 0, 0)})
	{
	}

private:
};

LAUNCH_RENDERWERK(FEditorApplication)
