#include <pch.h>
#include <Engine/Entity.h>

/* --------------------------------------------------------------------------------------------------------- */
Engine::CTransform::CTransform()
	: m_Position(), m_Rotation(), m_Scaling(1, 1, 1)
{
}

mat4 Engine::CTransform::GetWorldMatrix()
{
	return HMM_Transform(m_Position, m_Rotation, m_Scaling);
}

/* --------------------------------------------------------------------------------------------------------- */
uint _NextEntityID = 0;

Engine::CEntity::CEntity()
	: CTransform(), m_Name("New Entity")
{
	m_uId = _NextEntityID;
	_NextEntityID++;
}