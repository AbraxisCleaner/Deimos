#if !defined(_ENGINE_ENTITY_H_)
#define _ENGINE_ENTITY_H_

#include <pch.h>
#include <stl/String.h>
#include <stl/Container.h>
#include <../vendor/HandmadeMath.h>

namespace Engine
{
	struct CTransform
	{
		CTransform();
		mat4 GetWorldMatrix();

		vec3 m_Position;
		vec3 m_Rotation;
		vec3 m_Scaling;
	};

	struct IEntity : public CTransform {
		typedef CTransform Tf;

		IEntity();

		u32 m_uId;
		TSizedString<64> m_Name;
	};
}

#endif // _ENGINE_ENTITY_H_