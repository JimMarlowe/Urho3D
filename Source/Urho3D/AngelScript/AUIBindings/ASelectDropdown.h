#pragma once
#include <../AngelScript/Script.h>
#include <../ThirdParty/AngelScript/angelscript.h>
#include <../AngelScript/APITemplates.h>
#include <../AUI/ASelectDropdown.h>
#include "AButton.h"

namespace Bindings
{

using namespace Urho3D;

ASelectDropdown * wrapper_ASelectDropdown_constructor_createWidget (bool createWidget = true)
{
    return new ASelectDropdown (Urho3D::GetScriptContext (), createWidget);
}

template <class T> void RegisterASelectDropdown (asIScriptEngine *engine, const char *className, bool registerConstructors)
{
    RegisterAButton <T> (engine, className, false);

    if (registerConstructors)
    {
        Urho3D::RegisterSubclass <AButton, T> (engine, "AButton", className);

        engine->RegisterObjectBehaviour (className, asBEHAVE_FACTORY, (Urho3D::String (className) + "@+ f (bool createWidget = true)").CString (), asFUNCTION (wrapper_ASelectDropdown_constructor_createWidget), asCALL_CDECL);
    }

    engine->RegisterObjectMethod (className, "void SetSource (ASelectItemSource@+ source) ", asMETHOD (T, SetSource), asCALL_THISCALL);
}

}

