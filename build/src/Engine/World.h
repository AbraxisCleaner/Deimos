#if !defined(_ENGINE_WORLD_H_)
#define _ENGINE_WORLD_H_

#include <pch.h>
#include <Engine/Entity.h>
#include <Stl/Container.h>

namespace Engine
{
	struct CWorld
	{
		CWorld();
		~CWorld();

		float m_nAmbientColor[3];
		TArray<IEntity> m_EntityStorage;
	};
	extern CWorld *g_pWorld;
}

#endif // _ENGINE_WORLD_H_