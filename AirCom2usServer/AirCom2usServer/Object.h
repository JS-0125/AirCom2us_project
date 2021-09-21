#pragma once
#include "default.h"

class Object
{
public:
	int		m_id;
	int m_move_time=0;
	float m_x=0, m_y=0;
	atomic<OBJECT_STATE> m_state;
};

